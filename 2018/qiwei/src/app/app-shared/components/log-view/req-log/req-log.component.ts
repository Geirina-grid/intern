import { Component, OnInit, ViewEncapsulation } from '@angular/core';
import { BackLogService } from '../../../services/back-log.service';

@Component({
  selector: 'app-req-log',
  templateUrl: './req-log.component.html',
  styleUrls: ['./req-log.component.css'],
  encapsulation: ViewEncapsulation.None,
  providers: [BackLogService]
})
export class ReqLogComponent implements OnInit {

  public logs: string[];
  constructor(
    private backlogService: BackLogService,
  ) {
    this.backlogService.getlog('req.log')
      .then(resp => {
        this.logs = resp.split('\n');
      })
      .catch(this.errorHandler);
  }

  ngOnInit() {
  }
  errorHandler(error: any) {
    alert(error);
  }

}
