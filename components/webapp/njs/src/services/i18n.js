
import { getLocaleFromNavigator, init, addMessages, _ } from 'svelte-i18n';

const en = {
    "app": {
        "zone":"Zone",
        "duration":"Dur",
        "run":"Run",
        "remaining":"Rem",
        "pump":"Pump",
        "pressControl":"PC",
        "rainSensor":"Rain-Sensor",
        "reload":"Reload",
        "restartMcu":"Restart MCU",
        "save":"Save",
        "hide":"Hide",
        "zoneName":"Name",
        "nav_main_rv":"RV",
        "nav_main_config":"Config",
        "nav_main_firmware":"Firmware",
        "nav_fw_rv":"RV-MCU",
        "nav_fw_netMcu":"Net-MCU",
        "nav_cfg_rv":"RV-MCU",
        "nav_cfg_netMcu":"Net-MCU",
        "msg_waitForMcuRestart":"Wait for MCU to restart...",
        "msg_enterNewName":"Enter new name:",
        "msg_firmwareIsUpdating":"Firmware is updating...",
    },

  navTab: {
    main: {
      move: "\u25b3\u25bd",
      percent: "%%",
      config: "\u2699",
      auto: "Auto",
      firmware: "OTA",
      user_level: {
        developer: "\uD83D\uDDA5\uD83D\uDDA5",
        all: "$#",
        expert: "\uD83D\uDCBB",
        admin: "\uD83D\uDEE1",
        user: "\u2602",
        kid: "\uD83D\uDEB8",
        name: {
          developer: "Developer",
          expert: "Expert",
          admin: "Admin",
          user: "User",
          kid: "Kid",
        },
      },

      guiacc: {
        tab: "",
      },
    },

    help: {
      tab: "\u2753",
      ota: {
        tab: "OTA",
      },
      hsc: {
        tab: "\uD83C\uDFE0",
      },
      backup: {
        tab: "Backup",
      },
      doc: {
        tab: "\uD83D\uDCD6",
      },
    },

    cfg: {
      mcu: {
        tab: "MCU",
        network: { tab: "Network" },
        misc: { tab: "Misc" },
        cc1101: {
          tab: "CC1101",
        },
      },

      receiver: {
        tab: "Receiver",
        names: {
          tab: "Names",
        },
        durations: {
          tab: "Duration",
        },
        rot_dir: {
          tab: "Rotation-Direction",
        },

        endpos: {
          tab: "End-Positions",
        },

        set_mode: {
          tab: "SET",
          activate: {
            tab: "Function",
          },
          register_gm: {
            tab: "Centre",
          },
          register_tx: {
            tab: "Transmitter",
          },
        },
      },

      transmitter: {
        tab: "Transmitter",
        repeater: {
          tab: "Repeater",
        },
        register: {
          tab: "Register",
        },
        names: {
          tab: "Names",
          input: "TX-Name",
        },
      },
    },
  },

    

  panes: {
    ota: {
      header: "OTA Firmware Update",
      tt: {
        header: "Firmware information and Over the Air firmware update from GitHub zwiebert/tronferno-mcu-bin repository",
      },
    },

    backup: {
      header: "Backup/Restore Settings",
      create: "Create File",
      open: "Open File",
      load: "Load",
      restore: "Restore",
      toClipboard: "Copy to Clipboard",
      tt: {
        header: `Backup: 1) Click Create button to create a backup file on MCU. 2) Click the settings.json URL to open and save it. 
Restore: 1) Paste the content of the saved backup file into the text area 2) Click restore button`,
        create: "Create a backup file settings.json on the MCU. It can be downloaded by the URL next to this button",
        open: "Open a saved backup file and show it in text area",
        load: "Fetch the settings.json stored in MCU (!) and show it in the text area",
        restore: "Overwrite the MCU Settings by the JSON data in the text area",
        toClipboard: "Copy JSON from text area to clipboard",
      },
    },

    help_doc: {
      header: "Online Documentation",
      tt: {
        header: "Access documentation on GitHub",
      },
      starter: {
        url: "docs/starter-de.md",
        txt: "Getting Started",
      },
      starter_faq: {
        url: "docs/starter_faq-de.md",
        txt: "Starter FAQ",
      },

      mqtt: {
        url: "docs/mqtt.md",
        txt: "MQTT Client",
      },
    },

    network: { header: "Network connections and services", tt: { header: "--" } },
    misc: { header: "Other Settings", tt: { header: "--" } },
    cc1101: {
      header: "CC1101 RF-Module Configuration (For Developers)",
      tt: {
        header:
          "Configure CC1101 by providing a register file (ASCII-HEX string) or change a small subset of registers in the experimental configuration editor",
      },
    },

    rx_names: {
      header: "Receiver Names",
      tt: {
        header: "Edit group and receiver names",
      },
    },
    durations: {
      header: "Movement Durations",
      tt: {
        header:
          "Configure here the durations for the shutter to move from open to close and from open to sup-position. This is needed to calculate the current in-between shutter position and to reach a requested in-between position.",
      },
    },
    rot_dir: {
      header: "Motor Rotation Direction",
      tt: {
        header: `Direction of rotation. Change direction of rotation of a motor.
First check the current direction with arrow buttons. If wrong, then reverse the rotation with the button and check again`,
      },
    },

    endpos: {
      header: "End Position Setting",
      tt: {
        header: "Set lower end upper end position for Fernotron tubular-motors ",
      },
    },

    activate: {
      header: "Activate SET",
      tt: {
        header: `Activate SET-function of receiver. You have 60 seconds in which to
          register additional transmitters with the selected receiver.`,
      },
    },
    register_gm: {
      header: "Register Receiver with Programming Centre",
      tt: {
        header: `To assign a receiver to a group and give it a receiver number:
           1) active SET function of receiver. 2) transmit data (as in timer data) using the intended group/receiver-number`,
      },
    },
    register_tx: {
      header: "Register Receiver with Transmitter",
      tt: {
        header:
          "To register/unregister a transmitter to a receiver: 1) activate SET function of receiver. 2) send STOP command with transmitter -or- chose the transmitter ID here and press STOP",
      },
    },

    repeater: {
      header: "RF-Repeater",
      tt: {
        header:
          "Repeat plain RF transmitter commands by the MCU's RF transmitter to increase reach. You have to provide a list of transmitter-IDs. Only transmitters on this list will be repeated.",
      },
    },
    register: {
      header: "Register Transmitters",
      tt: {
        header: "To register position changes caused by plain controllers and sun-sensors, these controllers have to be registered here.",
      },
    },
    tx_names: {
      header: "Transmitter-Names",
      input: "TX-Name",
      tt: {
        header: "Assign names to transmitters for user convenience",
      },
    },

    hs_config: {
      home_assistant: {
        header: "Home Assistant Configuration",
        tt: {
          header: "1) Configures Home Assistant via MQTT discover or 2) Generates a configuration for Home Assistant for copy-and-paste",
        },
      },
      fhem: {
        header: "FHEM Configuration",
        tt: {
          header:
            "Generates device definitions for FHEM home-server. Definitions are generated for generic MQTT or the dedicated tronferno-fhem module. Paste the generated definitions into FHEMWEB and execute them there",
        },
      },
      openhab: {
        header: "OpenHAB Configuration",
        tt: {
          header: "Generates a configuration for OpenHAB home-server for copy-and-paste. Configurations are generated for MQTT-Generic-Thing",
        },
      },
    },

    guiacc: {
      header: "GUI Access restriction",
      tt: {
        header:
          "Experimental/Unfinished work: Reduce user level to hide inappropriate user interface areas (e.g. most configuration areas need at least admin level)",
      },
    },
  },


    
} ;


const de = {
    "app": {
        "zone":"Bereich",
        "duration":"Dauer",
        "run":"Ein",
        "remaining":"Rest",
        "pump":"Pumpe",
        "pressControl":"Druckschalter",
        "rainSensor":"Regen-Sensor",
        "reload":"Neu Laden",
        "restartMcu":"MCU Neustart",
        "save":"Speichern",
        "hide":"Verstecke",
        "msg_waitForMcuRestart":"MCU wird neu gestartet...",
        "msg_enterNewName":"Neuer Name:",
        "msg_firmwareIsUpdating":"Firmware aktualisiert sich...",
    }
} ;

function setupI18n() {
    addMessages('en', en);
    addMessages('de', de);

    init({
        fallbackLocale: 'en',
        initialLocale: getLocaleFromNavigator(),
      });
}


export { _, setupI18n };

