import { TestBed, inject } from '@angular/core/testing';

import { ModelingService } from './modeling.service';

describe('ModelingService', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [ModelingService]
    });
  });

  it('should be created', inject([ModelingService], (service: ModelingService) => {
    expect(service).toBeTruthy();
  }));
});
