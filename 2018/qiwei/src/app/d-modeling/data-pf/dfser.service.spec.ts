import { TestBed, inject } from '@angular/core/testing';

import { DfserService } from './dfser.service';

describe('DfserService', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [DfserService]
    });
  });

  it('should be created', inject([DfserService], (service: DfserService) => {
    expect(service).toBeTruthy();
  }));
});
