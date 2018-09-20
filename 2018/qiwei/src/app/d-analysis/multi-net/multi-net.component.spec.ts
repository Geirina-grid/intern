import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { MultiNetComponent } from './multi-net.component';

describe('MultiNetComponent', () => {
  let component: MultiNetComponent;
  let fixture: ComponentFixture<MultiNetComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ MultiNetComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(MultiNetComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
