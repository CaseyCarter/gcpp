
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016 Herb Sutter. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef GALLOC_GPAGE_ALLOCATOR
#define GALLOC_GPAGE_ALLOCATOR

#include "gpage.h"

namespace gcpp {

	//----------------------------------------------------------------------------
	//
	//	gpage_allocator - wrap a gpage as a C++14 allocator, with thanks to
	//			 Howard Hinnant's allocator boilerplate exemplar code, online at
	//           https://howardhinnant.github.io/allocator_boilerplate.html
	//
	//----------------------------------------------------------------------------

	template <class T>
	class gpage_allocator {
		gpage* page;

		static gpage& instance() {
			static gpage singleton;
			return singleton;
		}

	public:
		using value_type = T;

		gpage_allocator(gpage& page = instance()) : page(&page)
		{
		}

		template <class U>
		gpage_allocator(gpage_allocator<U> const& that) noexcept : page(that.page)
		{
		}

		value_type* allocate(std::size_t n) noexcept
		{
			if (n > max_size()) {
				throw std::bad_array_length();
			}
			if (auto result = page->allocate<T>(n)) {
				return reinterpret_cast<value_type*>(result);
			}
			throw std::bad_alloc();
		}

		void deallocate(value_type* p, std::size_t) noexcept
		{
			return page->deallocate(p);
		}

		std::size_type max_size() const noexcept
		{
			return std::size_type(-1) / sizeof(value_type);
		}

		template <class U, class V>
		friend bool operator==(gpage_allocator<U> const& x, gpage_allocator<V> const& y) noexcept;
	};

	template <class T, class U>
	bool operator==(gpage_allocator<T> const& x, gpage_allocator<U> const& y) noexcept
	{
		return x.page == y.page;
	}

	template <class T, class U>
	bool operator!=(gpage_allocator<T> const& x, gpage_allocator<U> const& y) noexcept
	{
		return !(x == y);
	}

}

#endif
