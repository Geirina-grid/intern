import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/Observable';
import { Subject } from 'rxjs/Subject';

@Injectable()
export class LoadforecastNavService {

  private pageIndex = new Subject<number>();

  // pageIndex$ = this.pageIndex.asObservable();

  // constructor() { }

  changeIndex (index:number) {
    this.pageIndex.next(index);
  }

  getIndex (): Observable<number> {
    return this.pageIndex.asObservable()
  }

}
