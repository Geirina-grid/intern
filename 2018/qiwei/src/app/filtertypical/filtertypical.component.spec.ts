import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { FiltertypicalComponent } from './filtertypical.component';

describe('FiltertypicalComponent', () => {
  let component: FiltertypicalComponent;
  let fixture: ComponentFixture<FiltertypicalComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ FiltertypicalComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(FiltertypicalComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
