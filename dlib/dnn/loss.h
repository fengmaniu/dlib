// Copyright (C) 2015  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_DNn_LOSS_H_
#define DLIB_DNn_LOSS_H_

#include "loss_abstract.h"
#include "core.h"
#include "../matrix.h"
#include "tensor_tools.h"

namespace dlib
{

// ----------------------------------------------------------------------------------------

    class loss_binary_hinge_ 
    {
    public:

        const static unsigned int sample_expansion_factor = 1;
        typedef float label_type;

        template <
            typename SUB_TYPE,
            typename label_iterator
            >
        void to_label (
            const tensor& input_tensor,
            const SUB_TYPE& sub,
            label_iterator iter
        ) const
        {
            const tensor& output_tensor = sub.get_output();
            DLIB_CASSERT(output_tensor.nr() == 1 && 
                         output_tensor.nc() == 1 && 
                         output_tensor.k() == 1,"");
            DLIB_CASSERT(input_tensor.num_samples() == output_tensor.num_samples(),"");

            const float* out_data = output_tensor.host();
            for (long i = 0; i < output_tensor.num_samples(); ++i)
            {
                *iter++ = out_data[i];
            }
        }

        template <
            typename const_label_iterator,
            typename SUBNET
            >
        double compute_loss (
            const tensor& input_tensor,
            const_label_iterator truth, 
            SUBNET& sub
        ) const
        {
            const tensor& output_tensor = sub.get_output();
            tensor& grad = sub.get_gradient_input();

            DLIB_CASSERT(input_tensor.num_samples() != 0,"");
            DLIB_CASSERT(input_tensor.num_samples()%sample_expansion_factor == 0,"");
            DLIB_CASSERT(input_tensor.num_samples() == grad.num_samples(),"");
            DLIB_CASSERT(input_tensor.num_samples() == output_tensor.num_samples(),"");
            DLIB_CASSERT(output_tensor.nr() == 1 && 
                         output_tensor.nc() == 1 && 
                         output_tensor.k() == 1,"");

            // The loss we output is the average loss over the mini-batch.
            const double scale = 1.0/output_tensor.num_samples();
            double loss = 0;
            const float* out_data = output_tensor.host();
            float* g = grad.host();
            for (long i = 0; i < output_tensor.num_samples(); ++i)
            {
                const float y = *truth++;
                DLIB_CASSERT(y == +1 || y == -1, "y: " << y);
                const float temp = 1-y*out_data[i];
                if (temp > 0)
                {
                    loss += scale*temp;
                    g[i] = -scale*y;
                }
            }
            return loss;
        }

        friend void serialize(const loss_binary_hinge_& , std::ostream& out)
        {
            serialize("loss_binary_hinge_", out);
        }

        friend void deserialize(loss_binary_hinge_& , std::istream& in)
        {
            std::string version;
            deserialize(version, in);
            if (version != "loss_binary_hinge_")
                throw serialization_error("Unexpected version found while deserializing dlib::loss_binary_hinge_.");
        }

    };

    template <typename SUBNET>
    using loss_binary_hinge = add_loss_layer<loss_binary_hinge_, SUBNET>;

// ----------------------------------------------------------------------------------------

    class loss_binary_log_ 
    {
    public:

        const static unsigned int sample_expansion_factor = 1;
        typedef float label_type;

        template <
            typename SUB_TYPE,
            typename label_iterator
            >
        void to_label (
            const tensor& input_tensor,
            const SUB_TYPE& sub,
            label_iterator iter
        ) const
        {
            const tensor& output_tensor = sub.get_output();
            DLIB_CASSERT(output_tensor.nr() == 1 && 
                         output_tensor.nc() == 1 && 
                         output_tensor.k() == 1,"");
            DLIB_CASSERT(input_tensor.num_samples() == output_tensor.num_samples(),"");

            const float* out_data = output_tensor.host();
            for (long i = 0; i < output_tensor.num_samples(); ++i)
            {
                *iter++ = out_data[i];
            }
        }


        template <
            typename const_label_iterator,
            typename SUBNET
            >
        double compute_loss (
            const tensor& input_tensor,
            const_label_iterator truth, 
            SUBNET& sub
        ) const
        {
            const tensor& output_tensor = sub.get_output();
            tensor& grad = sub.get_gradient_input();

            DLIB_CASSERT(input_tensor.num_samples() != 0,"");
            DLIB_CASSERT(input_tensor.num_samples()%sample_expansion_factor == 0,"");
            DLIB_CASSERT(input_tensor.num_samples() == grad.num_samples(),"");
            DLIB_CASSERT(input_tensor.num_samples() == output_tensor.num_samples(),"");
            DLIB_CASSERT(output_tensor.nr() == 1 && 
                         output_tensor.nc() == 1 && 
                         output_tensor.k() == 1,"");
            DLIB_CASSERT(grad.nr() == 1 && 
                         grad.nc() == 1 && 
                         grad.k() == 1,"");

            tt::sigmoid(grad, output_tensor);

            // The loss we output is the average loss over the mini-batch.
            const double scale = 1.0/output_tensor.num_samples();
            double loss = 0;
            float* g = grad.host();
            const float* out_data = output_tensor.host();
            for (long i = 0; i < output_tensor.num_samples(); ++i)
            {
                const float y = *truth++;
                DLIB_CASSERT(y == +1 || y == -1, "y: " << y);
                float temp;
                if (y > 0)
                {
                    temp = log1pexp(-out_data[i]);
                    loss += scale*temp;
                    g[i] = scale*(g[i]-1);
                }
                else
                {
                    temp = -(-out_data[i]-log1pexp(-out_data[i]));
                    loss += scale*temp;
                    g[i] = scale*g[i];
                }
            }
            return loss;
        }

        friend void serialize(const loss_binary_log_& , std::ostream& out)
        {
            serialize("loss_binary_log_", out);
        }

        friend void deserialize(loss_binary_log_& , std::istream& in)
        {
            std::string version;
            deserialize(version, in);
            if (version != "loss_binary_log_")
                throw serialization_error("Unexpected version found while deserializing dlib::loss_binary_log_.");
        }

    };

    template <typename SUBNET>
    using loss_binary_log = add_loss_layer<loss_binary_log_, SUBNET>;

// ----------------------------------------------------------------------------------------

}

#endif // DLIB_DNn_LOSS_H_

