import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { DevQComponent } from './dev-q.component';

describe('DevQComponent', () => {
  let component: DevQComponent;
  let fixture: ComponentFixture<DevQComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ DevQComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(DevQComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
