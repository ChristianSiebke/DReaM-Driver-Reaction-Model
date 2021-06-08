#pragma once

template <class... visitor>
struct variant_visitor : visitor...
{
    using visitor::operator()...;
};

template <class... visitor>
variant_visitor(visitor...) -> variant_visitor<visitor...>;
