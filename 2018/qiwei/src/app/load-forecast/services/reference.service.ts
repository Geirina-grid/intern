import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import {environment} from '../../../environments/environment';
import 'rxjs/add/operator/toPromise';

@Injectable()
export class ReferenceService {

    private headers = new Headers({ 'Content-Type': 'application/json' });
    private referenceUrl = environment.pyPre + '/reference/';  // URL to web api

    constructor(private http: Http) { }

    getReference(id: number, ts: number): Promise<any> {
        const url = `${this.referenceUrl}${id}/${ts}`;
        return this.http.get(url)
            .toPromise()
            .then(response => response.json().result)
            .catch(this.handleError);
    }

    getRangeReference(id: number, startdate: string, enddate: string): Promise<any> {
        const url = environment.pyPre + '/oridata/';
        return this.http
        .post(url, JSON.stringify({ meterid: id, start_date: startdate, end_date: enddate }), { headers: this.headers })
        .toPromise()
        .then(res => res.json())
        .catch(this.handleError);
    }

    getWeatherReference(startdate: string, enddate: string): Promise<any> {
        const url = environment.pyPre + '/addweatherdata/';
        return this.http
        .post(url, JSON.stringify({loadstart: startdate, loadend: enddate }), { headers: this.headers })
        .toPromise()
        .then(res => res.json().result)
        .catch(this.handleError);
    }

    getRealId(id: number): Promise<any> {
        const url = environment.pyPre + '/getrealmeterid/';
        return this.http
        .post(url, JSON.stringify({ meterid: id}), { headers: this.headers })
        .toPromise()
        .then(res => res.json())
        .catch(this.handleError);
    }

    getSimId(id: number): Promise<any> {
        const url = environment.pyPre + '/getsimmeterid/';
        return this.http
        .post(url, JSON.stringify({ meterid: id}), { headers: this.headers })
        .toPromise()
        .then(res => res.json())
        .catch(this.handleError);
    }

    getIdMatch(): Promise<any> {
        const url = environment.pyPre + '/idmatch/';
        return this.http
        .post(url, JSON.stringify({}), { headers: this.headers })
        .toPromise()
        .then(res => res.json())
        .catch(this.handleError);
    }

    getRawDataReferenceHighcharts(id: number, startdate: string, enddate: string): Promise<any> {
        const url = environment.pyPre + '/rawdataplot_highcharts/';
        return this.http
        .post(url, JSON.stringify({ meterid: id, start_date: startdate, end_date: enddate }), { headers: this.headers })
        .toPromise()
        .then(res => res.json())
        .catch(this.handleError);
    }

    getTypicalLoad(id: number, startdate: string, enddate: string): Promise<any> {
        const url = environment.pyPre + '/addtypicalload/';
        return this.http
        .post(url, JSON.stringify({ meterid: id, start_date: startdate, end_date: enddate }), { headers: this.headers })
        .toPromise()
        .then(res => res.json())
        .catch(this.handleError);
    }

    private handleError(error: any): Promise<any> {
        console.error('An error occurred', error); // for demo purposes only
        return Promise.reject(error.message || error);
    }
}

