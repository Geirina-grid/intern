import { Component, OnInit, Input, Output, EventEmitter } from '@angular/core';

@Component({
  selector: 'app-checklist',
  templateUrl: './checklist.component.html',
  styleUrls: ['./checklist.component.css']
})
export class ChecklistComponent implements OnInit {

  @Input() checkboxValue:boolean=false;
  @Input() listArray: any[];
  @Input() title: string;
  @Input() show: boolean;

  @Output() itemClickedEvent = new EventEmitter<string>();
  @Output() itemClickedEvent2 = new EventEmitter<string>();
  @Output() itemClickedEvent3 = new EventEmitter<string>();
  @Output() itemClickedEvent4 = new EventEmitter<string>();
  constructor() { }

  ngOnInit() {
  }

  getCheckList(): any[] {
    return this.listArray;
  }

  itemClicked(item){

    this.itemClickedEvent.emit(item);
  }
  
  itemClicked2(item2){

    this.itemClickedEvent2.emit(item2);
  }

  itemClicked3(){
    this.itemClickedEvent3.emit();
  }
  itemClicked4(){
    this.itemClickedEvent4.emit(this.checkboxValue.toString());
  }
}