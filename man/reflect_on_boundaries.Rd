% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/kernel.R
\name{reflect_on_boundaries}
\alias{reflect_on_boundaries}
\title{Reflective Boundaries}
\usage{
reflect_on_boundaries(x, lb, ub, which)
}
\arguments{
\item{x}{A numeric vector. The proposal}

\item{lb, ub}{Numeric vectors of length \code{length(x)}. Lower and upper bounds.}

\item{which}{Integer vector. Index of variables to be updated.}
}
\value{
An adjusted proposal vector.
}
\description{
Adjust a proposal according to its support by reflecting it. This is the workhorse
of \link{kernel_normal_reflective} and \link{kernel_unif_reflective}. It is intended
for internal use only.
}
