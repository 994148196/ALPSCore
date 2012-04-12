/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                 *
 * ALPS Project: Algorithms and Libraries for Physics Simulations                  *
 *                                                                                 *
 * ALPS Libraries                                                                  *
 *                                                                                 *
 * Copyright (C) 2010 - 2012 by Lukas Gamper <gamperl@gmail.com>                   *
 *                                                                                 *
 * This software is part of the ALPS libraries, published under the ALPS           *
 * Library License; you can use, redistribute it and/or modify it under            *
 * the terms of the license, either version 1 or (at your option) any later        *
 * version.                                                                        *
 *                                                                                 *
 * You should have received a copy of the ALPS Library License along with          *
 * the ALPS Libraries; see the file LICENSE.txt. If not, the license is also       *
 * available from http://alps.comp-phys.org/.                                      *
 *                                                                                 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT       *
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE       *
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,     *
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER     *
 * DEALINGS IN THE SOFTWARE.                                                       *
 *                                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef ALPS_NGS_DETAIL_GET_NUMPY_TYPE_HPP
#define ALPS_NGS_DETAIL_GET_NUMPY_TYPE_HPP

#include <alps/ngs/config.hpp>

#if !defined(ALPS_HAVE_PYTHON)
	#error numpy is only available if python is enabled
#endif

#include <alps/ngs/boost_python.hpp>
#include <alps/ngs/detail/numpy_import.hpp>

#include <complex>

namespace alps {
	namespace detail {

		int get_numpy_type(bool);
		int get_numpy_type(char);
		int get_numpy_type(unsigned char);
		int get_numpy_type(signed char);
		int get_numpy_type(short);
		int get_numpy_type(unsigned short);
		int get_numpy_type(int);
		int get_numpy_type(unsigned int);
		int get_numpy_type(long);
		int get_numpy_type(long long);
		int get_numpy_type(unsigned long long);
		int get_numpy_type(float);
		int get_numpy_type(double);
		int get_numpy_type(long double);
		int get_numpy_type(std::complex<float>);
		int get_numpy_type(std::complex<double>);
		int get_numpy_type(std::complex<long double>);

	}
}

#endif