import { Component, OnInit, ViewChild, AfterViewInit, OnChanges, SimpleChanges, Input } from '@angular/core';
import { Router } from '@angular/router';
import { ConfirmDialogModule } from 'primeng/primeng';

@Component({
  selector: 'app-newpopup',
  templateUrl: './newpopup.component.html',
  styleUrls: ['./newpopup.component.css']
})

export class NewpopupComponent implements OnInit {
  @Input() options1: any;
  @Input() options2: any;
  @Input() userinfo: any;
  @Input() clusterinfo: any;
  cols1 = [{ field: 'userid', header: '节点ID' },
  { field: 'percent', header: '比例' }];
  cols2 = [{ field: 'stat', header: '统计量' },
  { field: 'value', header: '数值' }];
  constructor(
    private router: Router,
  ) {
  }

  ngOnInit() {
  }
  onRowSelect(e) {
    // console.log(e)
    this.router.navigate(['data/clustering/' + e.data.userid])
  }
  onpieselect(e) {
    console.log(e)
    this.router.navigate(['data/clustering/' + e.context.name])
  }
}
