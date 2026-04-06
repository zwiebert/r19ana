/*
 * mqtt.c
 *
 *  Created on: 19.03.2019
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#ifdef CONFIG_APP_USE_MQTT

#include "app_mqtt/mqtt.h"

#include <string.h>
#include <stdio.h>
#include "cli/cli.h"
#include "cli/mutex.hh"
#include "uout/uout_builder_json.hh"
#include "app_uout/status_output.h"
#include <app_uout/callbacks.h>
#include <uout/uo_callbacks.h>
#include <net_mqtt_client/mqtt.hh>
#include <array>
#include <stdlib.h>

static char *io_mqtt_topic_root;

#define TOPIC_ROOT io_mqtt_topic_root

#define TOPIC_CMD_END "/duration"
#define TOPIC_CLI_END "/cli"


#define TOPIC_CLI  "cli"
#define TOPIC_STATUS  "status"
#define TOPIC_DUR_END "/duration"
#define TOPIC_DUR_MID "+"
#define TOPIC_CMD  TOPIC_DUR_MID TOPIC_DUR_END

#define TOPIC_CLI_OUT_END "cli_out"
#define TOPIC_CMD_OUT_END "cmd_out"
#define TOPIC_PCT_OUT_END "pct_out"
#define TOPIC_GPO_OUT_END "gpo_out"

#define TAG_CLI "cli "
#define TAG_CLI_LEN (sizeof(TAG_CLI) - 1)
#define TAG_SEND "cmd "
#define TAG_SEND_LEN (sizeof(TAG_SEND) - 1)
#define TAG_CONFIG "config "
#define TAG_CONFIG_LEN (sizeof(TAG_CONFIG) - 1)
#define TAG_TIMER "timer "
#define TAG_TIMER_LEN (sizeof(TAG_TIMER) - 1)

static void io_mqtt_publish_topic_end_get_json(const UoutWriter &td, const char *topic_end);
static void io_mqttApp_uoutPublish_cb(const uoCb_msgT msg);


static class AppNetMqtt final : public Net_Mqtt {

  virtual void received(const char *topic, int topic_len, const char *data, int data_len) override {
    UoutWriterBuilder td { SO_TGT_MQTT };
    if (!topic_startsWith(topic, topic_len, io_mqtt_topic_root)) {
      return; // all topics start with this
    }

    {
      LockGuard lock(cli_mutex);
      char line[40 + data_len];
      if (topic_endsWith(topic, topic_len, TOPIC_DUR_END)) {
        const char *addr = topic + strlen(TOPIC_ROOT);
        int addr_len = topic_len - (sizeof TOPIC_DUR_END - 1) - strlen(TOPIC_ROOT);
        sprintf(line, "cmd dur%.*s=%.*s", addr_len, addr, data_len, data);
        cli_process_cmdline(line, td);
      } else if (topic_endsWith(topic, topic_len, TOPIC_CLI_END)) {
        if (data_len > TAG_CLI_LEN && strncmp(data, TAG_CLI, TAG_CLI_LEN) == 0) {
          data += TAG_CLI_LEN;
          data_len -= TAG_CLI_LEN;
        }
        memcpy(line, data, data_len);
        line[data_len] = '\0';
        cli_process_cmdline(line, td);
        io_mqtt_publish_topic_end_get_json(td, TOPIC_CLI_OUT_END);

      } else if ((0 == strncmp(data, TAG_SEND, TAG_SEND_LEN)) || (0 == strncmp(data, TAG_CONFIG, TAG_CONFIG_LEN))
          || (0 == strncmp(data, TAG_TIMER, TAG_TIMER_LEN))) {
        memcpy(line, data, data_len);
        line[data_len] = '\0';
        cli_process_cmdline(line, td);
      }

      // RETURN:
    }
  }

  virtual void connected() override {
    char topic[64];

    // subscribe topics on MQTT server
    snprintf(topic, sizeof topic, "%scli", TOPIC_ROOT);
    subscribe(topic, 0);

    snprintf(topic, sizeof topic, "%s+/duration", TOPIC_ROOT);
    subscribe(topic, 0);

    // publish status change to MQTT server
    snprintf(topic, sizeof topic, "%sstatus", TOPIC_ROOT);
    publish(topic, "connected"); // for autocreate (ok???)

     uo_flagsT flags {};
     flags.tgt.mqtt = true;
     flags.evt.uo_evt_flag_valveChange = true;
     flags.fmt.raw = true;
     uoCb_subscribe(io_mqttApp_uoutPublish_cb, flags);
  }


  virtual void disconnected() override {
    uoCb_unsubscribe(io_mqttApp_uoutPublish_cb);
  }
} MyMqtt;

void io_mqttApp_setup(struct cfg_mqtt *cp) {
  io_mqtt_setup(cp);

  const char *topic_root = cp->root_topic;
  if (topic_root && *topic_root && (!io_mqtt_topic_root || 0 != strcmp(io_mqtt_topic_root, topic_root))) {
    free(io_mqtt_topic_root);
    if ((io_mqtt_topic_root = static_cast<char *>(malloc(strlen(topic_root) + 2)))) {
      strcat(strcpy(io_mqtt_topic_root, topic_root), "/");
    }
  }

  Net_Mqtt::setup(&MyMqtt);
}



static void io_mqtt_publish_topic_end(const char *topic_end, const char *json) {
  char topic[64];
  snprintf(topic, sizeof topic, "%s%s", TOPIC_ROOT, topic_end);

  MyMqtt.publish(topic, json);
}

static void io_mqtt_publish_topic_end_get_json(const UoutWriter &td, const char *topic_end) {
  char *json = td.sj().get_json();
  if (!json && !*json)
    return;
  io_mqtt_publish_topic_end(topic_end, json);
}


void io_mqtt_publish_config(const char *s)  {
    MyMqtt.publish("rv/config_out", s);
}

void io_mqtt_publish_valve_status(int valve_number, bool state) {
  char topic[64]; //, data[16];

  snprintf(topic, 64, "%szone/%d/valve", io_mqtt_topic_root, valve_number);

  MyMqtt.publish(topic, state ? "on" : "off");
}

void io_mqtt_publish_rain_sensor_status(bool state) {
  char topic[64]; //, data[16];

  snprintf(topic, 64, "%s%s/rain", io_mqtt_topic_root, TOPIC_STATUS);

  MyMqtt.publish(topic, state ? "on" : "off");
}

void io_mqtt_publish_pump_status(bool state) {
  char topic[64]; //, data[16];

  snprintf(topic, 64, "%s%s/pump", io_mqtt_topic_root, TOPIC_STATUS);

  MyMqtt.publish(topic, state ? "on" : "off");
}

void io_mqtt_publish_stm32_event(const char *event) {
  char topic[64]; //, data[16];

  snprintf(topic, 64, "%s%s/event", io_mqtt_topic_root, TOPIC_STATUS);

  MyMqtt.publish(topic, event);
}




static void io_mqttApp_uoutPublish_cb(const uoCb_msgT msg) {
  if (auto vs = uoCb_valveState_FromMsg(msg))
    io_mqtt_publish_valve_status(vs->valve_number, vs->is_open);
}



#endif // CONFIG_APP_USE_MQTT

