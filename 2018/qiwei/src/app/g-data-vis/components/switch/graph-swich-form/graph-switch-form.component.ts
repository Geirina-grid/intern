import { Component, Input, OnInit } from '@angular/core';
import { FormGroup } from '@angular/forms';

import { QuestionBase } from '../../../../app-shared/models/question-base';
import { QuestionControlService } from '../../../../app-shared/services/question-control.service';

@Component({
  selector: 'app-graph-switch-form',
  templateUrl: './graph-switch-form.component.html',
  styleUrls: ['./graph-switch-form.component.css'],
  providers: [QuestionControlService]
})
export class GraphSwitchFormComponent implements OnInit {

  @Input() questions: QuestionBase<any>[] = [];
  form: FormGroup;
  // payLoad = '';

  constructor(private qcs: QuestionControlService) { }

  ngOnInit() {
    this.form = this.qcs.toFormGroup(this.questions);
  }

  // onSubmit() {
  //   console.log(this.form.value);
  //   this.payLoad = JSON.stringify(this.form.value);
  // }
  // revert() {
  //   this.form.reset();
  // }
}
