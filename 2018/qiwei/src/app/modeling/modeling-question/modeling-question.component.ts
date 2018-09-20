import { Component, Input } from '@angular/core';
import { FormGroup } from '@angular/forms';
import { QuestionBase } from '../../app-shared/models/question-base';

@Component({
  selector: 'app-modeling-question',
  templateUrl: './modeling-question.component.html',
  styleUrls: ['./modeling-question.component.css']
})

export class ModelingQuestionComponent {
  @Input() question: QuestionBase<any>;
  @Input() form: FormGroup;
  get isValid() {
    console.log(this.form);
    return this.form.controls[this.question.key].valid; }
}

