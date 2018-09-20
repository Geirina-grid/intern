import { TestBed, inject } from '@angular/core/testing';

import { PfResultService } from './pf-result.service';

describe('PfResultService', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [PfResultService]
    });
  });

  it('should be created', inject([PfResultService], (service: PfResultService) => {
    expect(service).toBeTruthy();
  }));
});
