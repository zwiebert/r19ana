export interface ILabel {
  series_label: string;
  axis_label: string;
  range: number[];
}
export interface IModelInfo {
  name: string;
  description: string;
}
export interface Icar_chart {
  nmbGraphs: number;
  order: number[];
  labels: ILabel[];
  get_info(): IModelInfo;
  clear_chart_data(): void;
  get_chart_data(): (number | boolean)[][];
  get_labels: () => ILabel[];
  get_label(n: number): ILabel;
  get_nmb_of_graphs(): number;
  get_car_metrics(): Array<CarMetrics>;
  get_order(): number[];
  set_order(order: number[]): void;

  /**
   *
   * @param arr  a bunch of data frames in bendix bytestuffing format
   * @param ct   frame counter, if 0 and not append==true, reset chart first
   * @param append append to previous data, even it ct==0 (XXX: ???)
   */
  process_data_packet: (arr: Uint8Array, ct: number, append: boolean) => void;
}

export interface CarMetrics {
  k: number;
  parse: () => number | boolean;
  name: string;
  unit: string;
  range: number[];
  short_name: string;
}
