#pragma once

#include <thread>

#include <boost/asio.hpp>

#include "io_runner.hpp"
#include "output_writer.hpp"

namespace metrics {
  /**
   * The IORunner runs the async scheduler which powers the OutputWriter and all ContainerReaders,
   * while also acting as a factory for ContainerReaders.
   */
  class IORunnerImpl : public IORunner {
   public:
    /**
     * Creates an uninitialized instance. init() must be called before anything else.
     */
    IORunnerImpl();

    virtual ~IORunnerImpl();

    /**
     * Configures the IORunner with the provided parameters.
     * This must be called before any other methods.
     */
    void init(const mesos::Parameters& parameters);

    /**
     * Utility function to dispatch the provided method against the enclosed async scheduler.
     */
    void dispatch(std::function<void()> func);

    /**
     * Creates a new ContainerReader which is powered by an internal async scheduler for the
     * provided port. The returned ContainerReader won't have been open()ed yet.
     */
    std::shared_ptr<ContainerReader> create_container_reader(size_t port);

    /**
     * Submits an update to container resource state to the internal async scheduler.
     */
    void update_usage(process::Future<mesos::ResourceUsage> usage);

   private:
    void run_io_service();

    std::string listen_host;
    params::annotation_mode::Value annotation_mode;

    std::shared_ptr<boost::asio::io_service> io_service;
    std::vector<output_writer_ptr_t> writers;
    std::unique_ptr<std::thread> io_service_thread;
  };
}