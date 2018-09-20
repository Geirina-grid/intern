import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import {environment} from '../../../environments/environment';
import 'rxjs/add/operator/toPromise';


@Injectable()
export class NormalizerService {

    private headers = new Headers({ 'Content-Type': 'application/json' });
    private normalizerUrl = environment.pyPre + '/normalizer/';  // URL to web api

    constructor(private http: Http) { }


    getNormalizer(id: number): Promise<any[]> {
        const url = `${this.normalizerUrl}${id}`;
        return this.http.get(url)
            .toPromise()
            .then(response => response.json().result)
            .catch(this.handleError);
    }

    getNormalizers(): Promise<any[]> {
        return this.http.get(this.normalizerUrl)
        .toPromise()
        .then(responce => responce.json().result)
        .catch(this.handleError);

    }


    private handleError(error: any): Promise<any> {
        console.error('An error occurred', error); // for demo purposes only
        return Promise.reject(error.message || error);
    }
}

