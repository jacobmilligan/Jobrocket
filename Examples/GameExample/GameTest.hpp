//
//  GameTest.hpp
//  Tasks
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 29/11/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#pragma once

#include <GLFW/glfw3.h>

namespace tsk {

constexpr uint64_t kibibytes(const uint64_t amount)
{
    return 1024 * amount;
}

constexpr uint64_t mebibytes(const uint64_t amount)
{
    return (1024 * 1024) * amount;
}

constexpr uint64_t gibibytes(const uint64_t amount)
{
    return (1024 * 1024 * 1024) * amount;
}


class GraphicsDriver {
public:
    GraphicsDriver()
        : next_frame_(0)
    {
        for ( int i = 0; i < max_frames_; ++i ) {
            heaps_[i].init(mebibytes(2));
            frames_[i] = sky::Frame(i, 0.0, &heaps_[i], &cmd_bufs_[i]);
        }
    }

    sky::Frame get_frame()
    {
        auto frame = frames_[next_frame_];
        next_frame_ = (next_frame_ + 1) % max_frames_;
        frames_in_flight_ |= 1u << frame.get_id();
        return frame;
    }

    void complete_frame(const sky::Frame& frame)
    {
        frames_in_flight_ &= ~(1u << frame.get_id());
    }

    bool is_frame_active(const sky::Frame& frame)
    {
        return ((frames_in_flight_ >> frame.get_id()) & 1u) == 1u;
    }
private:
    static constexpr uint32_t max_frames_ = 15;

    uint32_t frames_in_flight_{0};
    uint32_t next_frame_{0};

    std::array<sky::TaggedProcessHeap, max_frames_> heaps_;
    std::array<sky::CommandBuffer, max_frames_> cmd_bufs_;
    std::array<sky::Frame, max_frames_> frames_;
};


class Application {
public:
    void run()
    {
        scheduler_.init();

        glfwSetErrorCallback(&Application::glfw_error_callback);

        if ( !glfwInit() ) {
            panic();
        }

        window_ = glfwCreateWindow(800, 600, "Game Test", nullptr, nullptr);

        if ( !window_ ) {
            panic();
        }

        glfwMakeContextCurrent(window_);
        glfwSetKeyCallback(window_, &Application::glfw_key_callback);

        glClearColor(0.42, 0.62, 0.94, 1.0);

        while ( !glfwWindowShouldClose(window_) ) {
            glClear(GL_COLOR_BUFFER_BIT);

            update();

            glfwSwapBuffers(window_);
            glfwPollEvents();
        }
    }

    void shutdown()
    {
        glfwDestroyWindow(window_);
        glfwTerminate();
        scheduler_.shutdown();
    }

    inline static sky::Scheduler* scheduler()
    {
        return &scheduler_;
    }
private:
    static void glfw_error_callback(int error, const char* description)
    {
        fputs(description, stderr);
    }

    static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS ) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    void panic()
    {
        shutdown();
        exit(EXIT_FAILURE);
    }

    void update()
    {

    }

    GLFWwindow* window_{nullptr};
    static sky::Scheduler scheduler_;
    GraphicsDriver gdi_;
};

sky::Scheduler Application::scheduler_;


}