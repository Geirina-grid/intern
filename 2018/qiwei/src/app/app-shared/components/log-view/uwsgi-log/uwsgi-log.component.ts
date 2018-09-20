import { Component, OnInit, ViewEncapsulation } from '@angular/core';
import { BackLogService } from '../../../services/back-log.service';

@Component({
  selector: 'app-uwsgi-log',
  templateUrl: './uwsgi-log.component.html',
  styleUrls: ['./uwsgi-log.component.css'],
  encapsulation: ViewEncapsulation.None,
  providers: [BackLogService]
})
export class UwsgiLogComponent implements OnInit {

  public logs: string[];
  constructor(
    private backlogService: BackLogService,
  ) {
    this.backlogService.getlog('uwsgi.log')
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
