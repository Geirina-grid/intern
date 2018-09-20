import { Injectable } from '@angular/core';
import { Subject }    from 'rxjs/Subject';
import { environment } from '../../../environments/environment';
@Injectable()
export class CommonService {

  // Observable string sources
  private loaddataParameterSource = new Subject<Object>();
  private loaddateReinitSource = new Subject<Object>();

  // Observable string streams
  loaddataParameterAnnounced$ = this.loaddataParameterSource.asObservable();
  loaddataReinitAnnounced$ = this.loaddateReinitSource.asObservable();

  public dict = {
    "Raw Data" : "原始负荷及天气数据一览",
    "Load(MW)" : "负荷(MW)",
    "Temperature(F)": "温度(F)",
    "Humidity(pct)": "湿度(pct)",
    "Windchill/Heatindex(F)": "风冷/酷热指数(F)",
    "Dewpoint(F)": "露点温度(F)",
    "Pressure(inch)": "气压(PSI)",
    "Wind(mph)": "风速(mph)",
    "rain_or_not": "是否下雨",
    "snow_or_not": "是否下雪",
    "thunderstorm_or_not": "是否雷暴",
    "fog_or_not": "是否有雾",
    "Weather Parameters": "天 气 参 数",
    "Load (MW)": "负荷(MW)",
    "Typical_Load(MW)": "基准负荷(MW)",
    "Substracted_Load(MW)": "剩余负荷(MW)",
    "devmean": "误差平均值",
    "devmedian": "误差中位数",
    "devmax": "误差最大值",
    "devmin": "误差最小值",
    "devstd": "误差平均值",
    "trainTime": "训练时间"
  };
  // Service message commands
  announceLoaddataParameter(parameter: Object) {
    this.loaddataParameterSource.next(parameter);
  }

  announceLoaddataReinit(parameter?: Object) {
    this.loaddateReinitSource.next(parameter);
  }

}
