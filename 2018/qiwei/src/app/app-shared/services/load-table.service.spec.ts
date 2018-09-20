import { TestBed, inject } from '@angular/core/testing';

import { LoadTableService } from './load-table.service';

describe('LoadTableService', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [LoadTableService]
    });
  });

  it('should be created', inject([LoadTableService], (service: LoadTableService) => {
    expect(service).toBeTruthy();
  }));
});
