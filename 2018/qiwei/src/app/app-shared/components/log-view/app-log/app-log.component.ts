import { Component, OnInit, ViewEncapsulation } from '@angular/core';
import { BackLogService } from '../../../services/back-log.service';

@Component({
  selector: 'app-app-log',
  templateUrl: './app-log.component.html',
  styleUrls: ['./app-log.component.css'],
  encapsulation: ViewEncapsulation.None,
  providers: [BackLogService]
})
export class AppLogComponent implements OnInit {

  public logs: string[];
  constructor(
    private backlogService: BackLogService,
  ) {
    this.backlogService.getlog('app.log')
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
