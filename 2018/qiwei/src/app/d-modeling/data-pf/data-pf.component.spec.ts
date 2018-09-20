import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { DataPfComponent } from './data-pf.component';

describe('DataPfComponent', () => {
  let component: DataPfComponent;
  let fixture: ComponentFixture<DataPfComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ DataPfComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(DataPfComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
