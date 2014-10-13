// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_TAKE_WHILE_HPP
#define RANGES_V3_VIEW_TAKE_WHILE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/range.hpp>
#include <range/v3/utility/unreachable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/view/transform.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng, typename Pred>
        struct take_while_view
          : range_adaptor<take_while_view<Rng, Pred>, Rng>
        {
        private:
            friend range_access;
            using reference_t = concepts::Invokable::result_t<Pred, range_value_t<Rng>>;
            semiregular_invokable_t<Pred> pred_;

            using single_pass = detail::or_t<
                SinglePass<range_iterator_t<Rng>>,
                detail::not_t<std::is_reference<reference_t>>>;

            struct sentinel_adaptor
              : adaptor_base
            {
            private:
                semiregular_invokable_ref_t<Pred> pred_;
            public:
                using single_pass = take_while_view::single_pass;
                sentinel_adaptor() = default;
                sentinel_adaptor(semiregular_invokable_ref_t<Pred> pred)
                  : pred_(std::move(pred))
                {}
                bool empty(range_iterator_t<Rng> it, range_sentinel_t<Rng> end) const
                {
                    return it == end || !pred_(*it);
                }
            };

            sentinel_adaptor end_adaptor() const
            {
                return {pred_};
            }
        public:
            take_while_view() = default;
            take_while_view(Rng && rng, Pred pred)
              : range_adaptor_t<take_while_view>{std::forward<Rng>(rng)}
              , pred_(invokable(std::move(pred)))
            {}
        };

        namespace view
        {
            struct take_while_fn : bindable<take_while_fn>
            {
                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(Iterable<Rng>())>
                static take_while_view<Rng, Pred>
                invoke(take_while_fn, Rng && rng, Pred pred)
                {
                    return {std::forward<Rng>(rng), std::move(pred)};
                }

                template<typename I, typename Pred,
                    CONCEPT_REQUIRES_(InputIterator<I>())>
                static take_while_view<range<I, unreachable>, Pred>
                invoke(take_while_fn, I begin, Pred pred)
                {
                    return {{std::move(begin), {}}, std::move(pred)};
                }

                template<typename Pred>
                static auto
                invoke(take_while_fn take_while, Pred pred) ->
                    decltype(take_while.move_bind(std::placeholders::_1, std::move(pred)))
                {
                    return take_while.move_bind(std::placeholders::_1, std::move(pred));
                }
            };

            RANGES_CONSTEXPR take_while_fn take_while{};
        }
    }
}

#endif
