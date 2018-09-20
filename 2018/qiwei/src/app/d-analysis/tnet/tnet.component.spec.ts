import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { TnetComponent } from './tnet.component';

describe('TnetComponent', () => {
  let component: TnetComponent;
  let fixture: ComponentFixture<TnetComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ TnetComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(TnetComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
