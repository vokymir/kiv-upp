#pragma once

// kazda varianta vypoctu vyplodi jednu ze 4 charakteristickych krivek DOPIPE
// paralelizace vyzkousejte si vsechny 4 varianty, zmerte a zapiste casy vypoctu
enum class ComputationVariant {
  LL,
  LG,
  GL,
  GG,
};

// prepinatko variant
constexpr ComputationVariant UsedVariant = ComputationVariant::GL;

// prepinatko paralelizace
constexpr bool IsParallel = true;
