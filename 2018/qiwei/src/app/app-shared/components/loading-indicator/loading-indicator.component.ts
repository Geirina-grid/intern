import { Component, Input } from '@angular/core';
import { trigger, state, style, animate, transition } from '@angular/animations';
import { ProgressSpinnerModule } from 'primeng/primeng';

const swiftEaseOut = trigger('swiftEaseOut', [
  state('1', style({opacity: 1})),
  state('0', style({opacity: 0})),
  transition('* => *', animate('400ms cubic-bezier(0.25, 0.8, 0.25, 1)'))
]);

@Component({
  selector: 'app-loading-indicator',
  templateUrl: './loading-indicator.component.html',
  styleUrls: ['./loading-indicator.component.css'],
  animations: [swiftEaseOut]
})
export class LoadingIndicatorComponent {

  @Input() show = false;
  @Input() styleString = "";
}
