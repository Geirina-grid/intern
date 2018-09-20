import { Component, OnInit, Input, Output, EventEmitter } from '@angular/core';

@Component({
  selector: 'app-data-table',
  templateUrl: './data-table.component.html',
  styleUrls: ['./data-table.component.css']
})
export class DataTableComponent implements OnInit {
  @Output() highlightedRows = [];
  @Input() dataSource: any[];
  @Input() columns: any[];
  @Input() displayedColumns: any[];

  constructor() { }


  ngOnInit() {
  }
  @Output() ClumnClicked = new EventEmitter<string>();

  checkClickStatus(row){
    var index = this.highlightedRows.indexOf(row)
    if(index != -1) {
       this.highlightedRows.splice(index,1);
    } else {
      this.highlightedRows.push(row);
    }
    console.log( this.highlightedRows)
    this.ClumnClickedEvent();
  }

  ClumnClickedEvent()
  {

    this.ClumnClicked.emit();

  }

}