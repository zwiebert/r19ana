export interface Icar_chart {
  get_info: () => any;
  clear_chart_data: () => void;
  get_chart_data: () => any[][];
  get_labels: () => any[];
  get_label: (n: number) => any;
  get_nmb_of_graphs: () => number;
  process_frame: (arr: Uint8Array, ct: number, append: boolean) => void;
}

export interface CarMetrics {
  k: number;
  parse: () => number | boolean;
  name: string;
  unit: string;
  range: number[],
  short_name: string;
}

export interface ILabel {series_label: string; axis_label: string; range:number[] }