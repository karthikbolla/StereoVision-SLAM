#ifndef FRONTEND_H
#define FRONTEND_H

#include<StereoVisionSLAM/common_include.h>
#include<StereoVisionSLAM/feature.h>
#include<StereoVisionSLAM/frame.h>
#include<StereoVisionSLAM/map.h>
#include<StereoVisionSLAM/camera.h>
#include<StereoVisionSLAM/viewer.h>
#include <opencv2/features2d.hpp>

namespace slam
{

    class Backend;
    class Viewer;

    enum class FrontendStatus { INITING, TRACKING_GOOD, TRACKING_BAD, LOST };

    class Frontend
    {
        public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
            typedef std::shared_ptr<Frontend> Ptr;

            Frontend();

            void SetMap(Map::Ptr map);

            void SetBackend(std::shared_ptr<Backend> backend);

            void SetViewer(std::shared_ptr<Viewer> viewer);
            
            FrontendStatus GetStatus() const;

        private:

            FrontendStatus status_{FrontendStatus::INITING};
            
            // Current input frame to track
            Frame::Ptr current_frame_{nullptr};
            // Last frame 
            Frame::Ptr last_frame_{nullptr};
            // Left camera of stereo system 
            Camera::Ptr camera_left_{nullptr};
            // Right Camera of stereo system
            Camera::Ptr camera_right_{nullptr};

            Map::Ptr map_{nullptr};
            std::shared_ptr<Backend> backend_{nullptr};
            std::shared_ptr<Viewer> viewer_{nullptr};

            // The relative motion between the current frame and 
            // the previous frame, used to estimate the initial 
            // pose of the current frame.
            Sophus::SE3d relative_motion_;

            // number of inlier for determining a frame is keyframe or not
            int tracking_inliers_{0};

            // Hyper-parameters

            // Maximum number of keypoint features to be extracted by
            // the keypoint feature detector
            int num_features_{200};
            // Minimum number of keypoint features required to initialize  
            // the frontend with a pair of left and right stereo images.
            int num_features_init_{100};
            int num_features_tracking_{50};
            int num_features_tracking_bad_{20};
            int num_features_needed_for_keyframe_{80};

            // Keypoint feature Detector
            cv::Ptr<cv::GFTTDetector> gftt_;

            // Detect keypoint features in current frame
            int DetectFeatures();

            // Track keypoint features from the left image in 
            // the right image using optical flow
            int FindFeaturesInRight();

            // Generate initial map by triangulation of matching points
            // in the left and right camera image pair
            bool BuildInitMap();

            // Try to initialize frontend with stereo images
            bool StereoInit();

            // Triangulate corresponding keypoint features in 
            // left and right images of current frame
            int TriangulateNewPoints();

            // Track last frame left image features in current frame
            // left image 
            int TrackLastFrame();

            // Estimate current frame pose
            int EstimateCurrentPose();
    };


}

#endif