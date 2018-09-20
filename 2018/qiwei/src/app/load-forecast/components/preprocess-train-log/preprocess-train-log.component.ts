import { Component, OnInit, OnDestroy,Input} from '@angular/core';

import { PreprocessTrainLogService } from '../../services/preprocess-train-log.service';
import { Subscription } from 'rxjs/Subscription';
import { CommonService } from '../../services/common.service'

@Component({
  selector: 'app-preprocess-train-log',
  templateUrl: './preprocess-train-log.component.html',
  styleUrls: ['./preprocess-train-log.component.css']
})
export class PreprocessTrainLogComponent implements OnInit {

  result_tr: string;
  result_tr2:any[] = [];
  status: any = [];
  result_preprocess: any;
  statusSubscription:Subscription;
  resultPreprocessSubscription:Subscription;
  resultTrSubscription:Subscription;
  resultTr2Subscription:Subscription;

  constructor(private preprocessTrainLogService: PreprocessTrainLogService, private commonService: CommonService) {
    this.statusSubscription = preprocessTrainLogService.status$.subscribe(msg => {
      if (msg != null) {this.status.push(msg)}
      else {this.status=[]}
      });
    this.resultPreprocessSubscription = preprocessTrainLogService.result_preprocess$.subscribe(msg => {this.result_preprocess=msg});
    this.resultTrSubscription = preprocessTrainLogService.result_tr$.subscribe(msg => {this.result_tr=msg});
    this.resultTr2Subscription = preprocessTrainLogService.result_tr2$.subscribe(msg => {
      this.result_tr2 = [];
      Object.keys(msg).forEach(element => {
        this.result_tr2.push(element + ": " + msg[element]);
      });
    })
  }

  ngOnInit() {
  }

  ngOnDestroy() {
    // unsubscribe to ensure no memory leaks
    this.statusSubscription.unsubscribe();
    this.resultPreprocessSubscription.unsubscribe();
    this.resultTrSubscription.unsubscribe();
    this.resultTr2Subscription.unsubscribe();
}

}
