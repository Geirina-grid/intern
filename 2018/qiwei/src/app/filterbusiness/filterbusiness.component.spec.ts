import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { FilterbusinessComponent } from './filterbusiness.component';

describe('FilterbusinessComponent', () => {
  let component: FilterbusinessComponent;
  let fixture: ComponentFixture<FilterbusinessComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ FilterbusinessComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(FilterbusinessComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
