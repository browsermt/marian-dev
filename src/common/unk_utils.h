#include "data/alignment.h"
#include "data/types.h"
#include <string>

namespace marian {
  namespace utils {
    void replaceUnknownsFromSource(const Words &target,
                                   const std::vector<size_t> &sourceUnkPositions,
                                   const std::vector<string_view> &sourceByteRanges,
                                   const data::WordAlignment &wordAlignment,
                                   std::string &decoded,
                                   std::vector<string_view> &decodedByteRanges
                                   ){

      ABORT_IF(sourceUnkPositions.size() == 0, "It is pointless to copy string if there are no unknowns in between to be replaced.");
      std::string decodedWithReplacement;
      std::vector<std::pair<size_t, size_t>> replacedByteRanges;

      decodedWithReplacement.reserve(decoded.size());
      auto Copy = [&decodedWithReplacement, &replacedByteRanges](
                      const char *endOfLastWord, string_view &original, string_view &copyFrom) {
        size_t begin, end;
        // Fill prefix
        decodedWithReplacement += std::string(endOfLastWord, original.data() - endOfLastWord);

        // Insert the word and add char* agnostic byteRanges
        begin = decodedWithReplacement.size();
        decodedWithReplacement += std::string(copyFrom.data(), copyFrom.size());
        end = decodedWithReplacement.size();
        replacedByteRanges.emplace_back(begin, end);
      };

      // Pointers to construct decodedWithReplacement incrementally.
      size_t activeTgtPos = 0;
      const char *endOfLastWord = decoded.data();

      // wordAlignment = vector<Point>,  where Point = (srcPos, tgtPos, prob);
      // This vector is obtained by filtering prob based on a threshold.
      // If threshold = 1.0f, points are argmax of whatever the word maps to.

      // These are expected to be sorted by tgtPos, not srcPos (which is marian default);
      for(auto &point : wordAlignment) {
        // Clear decoded-string until now.

        while(activeTgtPos < point.tgtPos) {
          string_view original = decodedByteRanges[activeTgtPos];
          string_view copyFrom = original;
          Copy(endOfLastWord, original, copyFrom);

          // Increment pointers
          endOfLastWord = original.data() + original.size();
          ++activeTgtPos;
        }

        // We check the source for unknowns and always replace the target
        // irrespective of whether unknown or not. This has to be done after the
        // decoding, as we are copying content as is from source, which is
        // unknown and cannot be decoded otherwise.
        if(std::binary_search(sourceUnkPositions.begin(), sourceUnkPositions.end(), point.srcPos)) {
          string_view original = decodedByteRanges[activeTgtPos];
          string_view copyFrom = sourceByteRanges[point.srcPos];
          Copy(endOfLastWord, original, copyFrom);

          // Increment pointers
          endOfLastWord = original.data() + original.size();
          ++activeTgtPos;
        } else {
          // This need not be handled, the next prefix clear will take care of this automatically.
        }
      }

      // Flush the remaining
      while(activeTgtPos < decodedByteRanges.size()) {
        string_view original = decodedByteRanges[activeTgtPos]; 
        string_view copyFrom = original;
        Copy(endOfLastWord, original, copyFrom);

        // Increment pointers
        endOfLastWord = original.data() + original.size();
        ++activeTgtPos;
    }

    // Rebase string-views
    decoded = std::move(decodedWithReplacement);
    decodedByteRanges.clear();
    for(auto &p: replacedByteRanges){
      const char *data = &(decoded[p.first]);
      size_t size = p.second - p.first;
      decodedByteRanges.push_back(string_view(data, size));
    }
    }
  }
}