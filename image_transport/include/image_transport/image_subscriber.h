/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2009, Willow Garage, Inc.
*  All rights reserved.
* 
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
* 
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

#include <ros/ros.h>
#include <sensor_msgs/Image.h>

namespace image_transport {

/**
 * \brief Manages a subscription callback on a specific topic that can be interpreted
 * as an Image topic.
 *
 * ImageSubscriber is the client-side counterpart to ImagePublisher. By loading the
 * appropriate plugin, it can interpret any topic advertised by ImagePublisher as
 * Image messages passed to the user callback; the complexity of what transport is
 * actually used is hidden.
 *
 * Once all copies of a specific ImageSubscriber go out of scope, the subscription callback
 * associated with that handle will stop being called. Once all ImageSubscriber for a given
 * topic go out of scope the topic will be unsubscribed.
 *
 * @todo Add either SubscribeOps overload of subscribe() or CallbackQueueInterface arg to
 * other versions.
 */
class ImageSubscriber
{
public:
  ImageSubscriber();

  ImageSubscriber(const ImageSubscriber& rhs);

  ~ImageSubscriber();

  //void subscribe(ros::NodeHandle& nh, ros::SubscribeOptions& ops);

  /**
   * \brief Subscribe to a topic, version for arbitrary boost::function object.
   */
  void subscribe(ros::NodeHandle& nh, const std::string& topic, uint32_t queue_size,
                 const boost::function<void(const sensor_msgs::ImageConstPtr&)>& callback,
                 const ros::VoidPtr& tracked_object = ros::VoidPtr(),
                 const ros::TransportHints& transport_hints = ros::TransportHints());

  /**
   * \brief Subscribe to a topic, version for bare function.
   */
  void subscribe(ros::NodeHandle& nh, const std::string& topic, uint32_t queue_size,
                 void(*fp)(const sensor_msgs::ImageConstPtr&),
                 const ros::TransportHints& transport_hints = ros::TransportHints())
  {
    subscribe(nh, topic, queue_size,
              boost::function<void(const sensor_msgs::ImageConstPtr&)>(fp),
              ros::VoidPtr(), transport_hints);
  }

  /**
   * \brief Subscribe to a topic, version for class member function with bare pointer.
   */
  template<class T>
  void subscribe(ros::NodeHandle& nh, const std::string& topic, uint32_t queue_size,
                 void(T::*fp)(const sensor_msgs::ImageConstPtr&), T* obj,
                 const ros::TransportHints& transport_hints = ros::TransportHints())
  {
    subscribe(nh, topic, queue_size, boost::bind(fp, obj, _1), ros::VoidPtr(), transport_hints);
  }

  /**
   * \brief Subscribe to a topic, version for class member function with shared_ptr.
   */
  template<class T>
  void subscribe(ros::NodeHandle& nh, const std::string& topic, uint32_t queue_size,
                 void(T::*fp)(const sensor_msgs::ImageConstPtr&),
                 const boost::shared_ptr<T>& obj,
                 const ros::TransportHints& transport_hints = ros::TransportHints())
  {
    subscribe(nh, topic, queue_size, boost::bind(fp, obj.get(), _1), obj, transport_hints);
  }

  std::string getTopic() const;

  /**
   * \brief Unsubscribe the callback associated with this ImageSubscriber.
   */
  void shutdown();

  operator void*() const;
  bool operator< (const ImageSubscriber& rhs) const { return impl_ <  rhs.impl_; }
  bool operator!=(const ImageSubscriber& rhs) const { return impl_ != rhs.impl_; }
  bool operator==(const ImageSubscriber& rhs) const { return impl_ == rhs.impl_; }
  
private:
  struct Impl;
  boost::shared_ptr<Impl> impl_;
};

} //namespace image_transport
