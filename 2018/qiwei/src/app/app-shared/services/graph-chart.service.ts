import { Injectable } from '@angular/core';
import { Subject } from 'rxjs/Subject';
import { Observable } from 'rxjs/Observable';
import { GraphChart } from 'gvis';
import 'rxjs/add/operator/share';

@Injectable()
export class GraphChartService {
  graphChartSource = new Subject<any>();
  graphChartUpdateSource = new Subject<any>();
  subscriptionSource = new Subject<any>();
  graphInitiatedSource = new Subject<boolean>();
  mapInitiatiatedSource = new Subject<boolean> ();
  mapSubscriptionSource = new Subject<any>();
  mapSource = new Subject<any>();
  mapUpdateSource = new Subject<any>();

  graphChart$ = this.graphChartSource.asObservable();
  graphChartUpdate$ = this.graphChartUpdateSource.asObservable();
  newSubscription$ = this.subscriptionSource.asObservable();
  graphInitiated$ = this.graphInitiatedSource.asObservable();
  mapInitiated$ = this.mapInitiatiatedSource.asObservable();
  map$ = this.mapSource.asObservable();
  mapUpdate$ = this.mapUpdateSource.asObservable();
  newMapSubscription$ = this.mapSubscriptionSource.asObservable();
  test: String;
  constructor() {
  }

  announceGraphchart(graphchart: any) {
    this.graphChartSource.next(graphchart);
    console.log(graphchart);
    console.log('Chart announced');
  }

  announceGrapchartUpdate(isNotEmpty = true) {
    this.graphChartUpdateSource.next(isNotEmpty);
  }

  announceSubscription(isNotEmpty = true) {
    this.subscriptionSource.next(isNotEmpty);
  }

  announceInitiation(isNotEmpty) {
    this.graphInitiatedSource.next(true);
  }

  announceMap(map: any) {
    this.mapSource.next(map);
    console.log(map);
    console.log('Map announced');
  }

  announceMapSubscription(isNotEmpty = true) {
    this.mapSubscriptionSource.next(isNotEmpty);
  }
  announceMapInitiation(isNotEmpty) {
    this.mapInitiatiatedSource.next(true);
  }
  announceMapUpdated(isNotEmpty = true) {
    this.mapUpdateSource.next(isNotEmpty);
  }
}
