import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import { environment } from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

@Injectable()
export class ModelService {

    private headers = new Headers({ 'Content-Type': 'application/json' });
    private pyPre = environment.pyPre;
    private modelUrl = this.pyPre + '/model/';  // URL to web api

    constructor(private http: Http) { }

    getModels(): Promise<any[]> {
        const url = `${this.modelUrl}`;
        return this.http.get(url)
            .toPromise()
            .then(response => response.json().result as any[])
            .catch(this.handleError);
    }

    getModel(id: number): Promise<any[]> {
        const url = `${this.modelUrl}${id}`;
        return this.http.get(url)
            .toPromise()
            .then(response => response.json().result as any[])
            .catch(this.handleError);
    }


    private handleError(error: any): Promise<any> {
        console.error('An error occurred', error); // for demo purposes only
        // const message = 'Seems like you don`t have a model for current System/Company/Meter.  You may want to PREPROCESS one first';
        // alert(message);
        return Promise.reject(error.message || error);
    }
}

