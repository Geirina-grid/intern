import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { ChecklistComponent } from './components/checklist/checklist.component';

// Angular Material Modules
import { MatButtonModule, MatCardModule, MatListModule, MatInputModule, MatSortModule} from '@angular/material';
import { MatCheckboxModule } from '@angular/material/checkbox';
import { MatTableModule } from '@angular/material/table';
import { MatAutocompleteModule } from '@angular/material/autocomplete';
import { MatChipsModule } from '@angular/material/chips';
import { MatIconModule } from '@angular/material/icon';

import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { DataTableComponent } from './components/data-table/data-table.component';



@NgModule({
  imports: [
    CommonModule,
    MatButtonModule, MatCheckboxModule, MatCardModule, MatListModule, MatInputModule, MatSortModule,
    MatTableModule, MatAutocompleteModule, MatChipsModule, MatIconModule,
    FormsModule, ReactiveFormsModule
  ],
  declarations: [ChecklistComponent, DataTableComponent],
  exports: [
    ChecklistComponent, DataTableComponent
  ]
})
export class MySharedModule { }