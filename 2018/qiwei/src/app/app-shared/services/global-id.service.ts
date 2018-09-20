import { Injectable } from '@angular/core';
import { Subject } from 'rxjs/Subject';
import { BehaviorSubject } from 'rxjs/BehaviorSubject'

@Injectable()
export class GlobalIdService {

  private idSource = new BehaviorSubject<number>(1);

  id$ = this.idSource.asObservable();

  announceId(id: number) {
    // console.log('announce selected id ' + id);
    this.idSource.next(id);
  }

}
