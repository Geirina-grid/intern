
import { Component, Input } from '@angular/core';
import { FormGroup } from '@angular/forms';
import { QuestionBase } from '../../../models/question-base';

@Component({
  selector: 'app-graph-manage-question',
  templateUrl: './graph-manage-question.component.html',
  styleUrls: ['./graph-manage-question.component.css']
})
export class GraphManageQuestionComponent {
  @Input() question: QuestionBase<any>;
  @Input() form: FormGroup;
  get isValid() {
    console.log(this.form);
    return this.form.controls[this.question.key].valid; }
}

