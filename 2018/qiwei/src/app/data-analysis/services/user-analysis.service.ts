import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import { environment } from '../../../environments/environment';

import 'rxjs/add/operator/toPromise';

@Injectable()
export class UserAnalysisService {
  private getCategorySummary = environment.pyPre + '/user_category_summary/'
  private getLoadShapeSummary = environment.pyPre + '/load_shape_summary/'
  private getCategoryTypical = environment.pyPre + '/category_typical_line/'
  private getCategoryLoadshape = environment.pyPre + '/category_load_shape/'
  private getCategoryEntropy = environment.pyPre + '/category_entropy/'
  private getCategoryUserUsageList = environment.pyPre + '/category_user_usage_list/'
  private getCategoryUserUsage = environment.pyPre + '/category_user_usage/'
  private headers = new Headers({ 'Content-Type': 'application/json' });
  constructor(private http: Http) { }

  PlotCategorySummary(): Promise<any> {
    const loadUrl = this.getCategorySummary;
    return this.http
      .post(loadUrl, JSON.stringify({ }), { headers: this.headers })
      .toPromise()
      .then(res => res.json().result)
      .catch(this.handleError);
  }
  PlotLoadShapeSummary(): Promise<any> {
    const loadUrl = this.getLoadShapeSummary;
    return this.http
      .post(loadUrl, JSON.stringify({ }), { headers: this.headers })
      .toPromise()
      .then(res => res.json().result)
      .catch(this.handleError);
  }
  PlotCategoryTypical(cate: string): Promise<any> {
    const loadUrl = this.getCategoryTypical;
    return this.http
      .post(loadUrl, JSON.stringify({ category: cate}), { headers: this.headers })
      .toPromise()
      .then(res => res.json().result)
      .catch(this.handleError);
  }
  PlotCategoryLoadshape(cate: string): Promise<any> {
    const loadUrl = this.getCategoryLoadshape;
    return this.http
      .post(loadUrl, JSON.stringify({ category: cate }), { headers: this.headers })
      .toPromise()
      .then(res => res.json().result)
      .catch(this.handleError);
  }
  PlotCategoryEntropy(cate: string): Promise<any> {
    const loadUrl = this.getCategoryEntropy;
    return this.http
      .post(loadUrl, JSON.stringify({ category: cate }), { headers: this.headers })
      .toPromise()
      .then(res => res.json().result)
      .catch(this.handleError);
  }

  PlotCategoryUserUsage(cate: string): Promise<any> {
    const loadUrl = this.getCategoryUserUsage;
    return this.http
      .post(loadUrl, JSON.stringify({ category: cate }), { headers: this.headers })
      .toPromise()
      .then((res) => res.json().result)
      .catch(this.handleError);
  }

  PlotCategoryUserUsageList(cate: string): Promise<any> {
    const loadUrl = this.getCategoryUserUsageList;
    return this.http
      .post(loadUrl, JSON.stringify({ category: cate }), { headers: this.headers })
      .toPromise()
      .then((res) => res.json().result)
      .catch(this.handleError);
  }

  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error); // for demo purposes only
    return Promise.reject(error.message || error);
  }
}
