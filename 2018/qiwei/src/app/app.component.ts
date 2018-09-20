import { Component } from '@angular/core';
import { WholeGraphService } from './app-shared/services/whole-graph.service';
import { Subscription } from 'rxjs/Subscription';
import {FormBuilder, FormGroup, Validators} from '@angular/forms';
@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent {
  title = '综合能源服务技术支持系统';

  subscriptions: Subscription[] = [];
  nodeLabels: any[] = [];
  edgeLabels: any[] = [];
  newSub: boolean;
  constructor(
    private service: WholeGraphService
  ) {
    console.log('constructor ran haha...')
    this.nodeLabels = ['id'];
    this.edgeLabels = [];
    this.subscriptions.push(this.service.newSubscription$.subscribe(
      (status) => {
        this.newSub = status;
        console.log('new label subscription');
        this.service.announceNodeLabel(this.nodeLabels);
        this.service.announceLinkLabel(this.edgeLabels);
      }
    ));
    this.subscriptions.push(this.service.nodelabelAnnounced$.subscribe(
      (labels) => {
        this.nodeLabels = labels;
        console.log(this.nodeLabels);
      }
    ));
    this.subscriptions.push(this.service.linklabelAnnounced$.subscribe(
      (labels) => {
        this.edgeLabels = labels;
        console.log(this.edgeLabels);
      
      }
    ));
  }
}
