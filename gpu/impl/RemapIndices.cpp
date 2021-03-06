/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD+Patents license found in the
 * LICENSE file in the root directory of this source tree.
 */

// Copyright 2004-present Facebook. All Rights Reserved.

#include "RemapIndices.h"
#include "../../FaissAssert.h"

namespace faiss { namespace gpu {

// Utility function to translate (list id, offset) to a user index on
// the CPU. In a cpp in order to use OpenMP
void ivfOffsetToUserIndex(
  long* indices,
  int numLists,
  int queries,
  int k,
  const std::vector<std::vector<long>>& listOffsetToUserIndex) {
  FAISS_ASSERT(numLists == listOffsetToUserIndex.size());

#pragma omp parallel for
  for (int q = 0; q < queries; ++q) {
    for (int r = 0; r < k; ++r) {
      long offsetIndex = indices[q * k + r];

      if (offsetIndex < 0) continue;

      int listId = (int) (offsetIndex >> 32);
      int listOffset = (int) (offsetIndex & 0xffffffff);

      FAISS_ASSERT(listId < numLists);
      auto& listIndices = listOffsetToUserIndex[listId];

      FAISS_ASSERT(listOffset < listIndices.size());
      indices[q * k + r] = listIndices[listOffset];
    }
  }
}

} } // namespace
