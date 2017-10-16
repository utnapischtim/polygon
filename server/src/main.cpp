#include <iostream>
#include <thread>
#include <string>
#include <csignal>
#include <algorithm>
#include <experimental/filesystem>

#include "docopt.h"
#include "easylogging++.h"

#include "WebsocketServer.h"
#include "Worker.h"
#include "Filter.h"
#include "Point.h"
#include "Generator.h"
#include "CommonSetting.h"
#include "Output.h"
#include "Statistics.h"

namespace docopt {
  using Arguments = std::map<std::string, docopt::value>;
}

namespace fs = std::experimental::filesystem;

INITIALIZE_EASYLOGGINGPP

pl::WebSocketServer websocket_server;
pl::Worker worker;

static void initWebsocketServer(int port);
static void initWorker();
static void terminate(int);
static void server(docopt::Arguments &args);
static void run(docopt::Arguments &args);
static std::string buildCustomFilename(docopt::Arguments &args);
static void collectSimpleStats(std::function<void(docopt::Arguments &)> func, docopt::Arguments &args);


// ATTENTION
// no empty line allowed between options, therefore no grouping possible
static const char USAGE[] =
R"(polygon generation
  USAGE:
    polygon --server [--port=PORT --v=K]
    polygon (--list-generator | --list-filter | --list-common-setting | --list-output-format)
    polygon --generator=KEY [--reflex-points=K --convex-points=K --reflex-chain=K --convex-chain=K --reflex-angle-range=R --convex-angle-range=R --lights-to-illuminate=K --nodes=NODES --sampling-grid=AREA --phases=P --radius=R --bouncing-radius=R --segment-length=L --output-format=FORMAT --output-dir=DIR --v=K --statistics --animation=A] (--file=FILE | --file-base=FILE_BASE)
    polygon (-h | --help)
    polygon --version

  OPTIONS:
    -h --help                 Show this screen.
    --server                  use the program as a server
    --port=PORT               port to run the server [default: 9000]
    --list-generator          list all generators
    --list-filter             list all filters
    --list-common-setting     list all common settings
    --list-output-format      list all output formats
    --generator=KEY           set the generator, the key is the given key shown in --list-generator
    --reflex-points=K         set the number of reflex points that should be at least. [default: 0]
    --convex-points=K         set the number of convex points that should be at least. [default: 0]
    --reflex-chain=K          set the number of reflex points in a chain. There could be more
                              reflex points in other chains, or in common more reflex points. it
                              could be combined with --reflex-points. should there be a number
                              less than points in the chain, than the chain has higher priority.
                              [default: 0]
    --convex-chain=K          the same as in --reflex-points-in-chain but with convex points.
                              [default: 0]
    --reflex-angle-range=R    set the range of the reflex angle. the angle is interpreted as open
                              in mathematical sense (180..360). [default: 180..360]
    --convex-angle-range=R    set the range of the convex angle. the angle is interpreted as open
                              in mathematical sense (0..180). [default: 0..180]
    --lights-to-illuminate=K  don't know what i meant. [default: 0]
    --nodes=NODES             how many nodes the polygon have to have. [default: 100]
    --sampling-grid=AREA      the area within the polygon could grow. [default: 1500x800]
    --segment-length=L        set the segment length. [default: 0]
    --phases=P                how many phases a random algorithm should run. [default: 10]
    --radius=R                the radius for regular polygon. [default: 60]
    --bouncing-radius=R       the distance to move a random point from old to new point.
                              [default: 60]
    --output-format=FORMAT    set the output format. [default: gnuplot]
    --output-dir=DIR          set the output dir. [default: .]
    --file=FILE               set the file.
    --file-base=FILE_BASE     set the file base. file format is base-generator-nodes.file_extension
    --statistics              output of simple statistics.
    --animation=A             create a animation of the different phases of bouncing vertices.
                              default is 0 for false, to start the creation set this to 1.
                              [default: 0]

    --v=K                     set the verbosity level. [default: 0]
)";

int main(int argc, char *argv[]) {
  // to set the verbosity level
  START_EASYLOGGINGPP(argc, argv);

  docopt::Arguments args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "0.1");

  if (args["--server"].asBool())
    server(args);

  else if (args["--list-generator"].asBool())
    pl::printGenerators();

  else if (args["--list-filter"].asBool())
    pl::printFilters();

  else if (args["--list-common-setting"].asBool())
    pl::printCommonSettings();

  else if (args["--list-output-format"].asBool())
    pl::printOutputFormats();

  else {
    if (auto dir = fs::status(args["--output-dir"].asString()); !fs::is_directory(dir)) {
      std::cout << "the directory given by --output-dir is not a existing directory\n";
      return -1;
    }

    if (args["--statistics"].asBool())
      collectSimpleStats(run, args);
    else
      run(args);
  }

  return 0;
}

void run(docopt::Arguments &args) {
  // it is guaranted by docopt that --generator has a value
  pl::Generator chosen_generator = pl::Generator(args["--generator"].asLong());
  pl::CommonSettingList common_settings = pl::createCommonSettingList(args);
  pl::FilterList filters = pl::createFilterList(args);
  pl::PointList list = pl::generatePointList(chosen_generator, common_settings, filters);

  std::string filename;
  if (args["--file"])
    filename = args["--file"].asString();
  else {
    filename = buildCustomFilename(args);
  }

  filename = args["--output-dir"].asString() + "/" + filename;

  pl::output(list, args["--output-format"].asString(), filename);

  // used as long for easier use in CommonSetting
  if (args["--animation"].asLong() == 1) {
    filename = filename.replace(filename.size() - 3, 3, "gif");
    filename = args["--output-dir"].asString() + "/" + filename;
    pl::output({}, "animation", filename);
  }
}

void server(docopt::Arguments &args) {
  int port = args["--port"].asLong();

  std::thread websocket_server_thread;

  if (port > 0)
    websocket_server_thread = std::thread(initWebsocketServer, port);

  std::thread worker_thread(initWorker);

  std::signal(SIGTERM, terminate);

  worker_thread.join();
  websocket_server_thread.join();
}

void initWebsocketServer(int port) {
  pthread_setname_np(pthread_self(), "WebsocketServer");
  websocket_server.init(port);
}

void initWorker() {
  pthread_setname_np(pthread_self(), "worker");
  worker.run();
}

void terminate(int) {
  std::terminate();
}

std::string buildCustomFilename(docopt::Arguments &args) {
  pl::Generator generator = pl::createGenerator(args["--generator"].asLong());

  std::string
    base = args["--file-base"].asString(),
    method = generator.name,
    nodes = args["--nodes"].asString(),
    reflex = args["--reflex-points"].asString(),
    convex = args["--convex-points"].asString(),
    file_extension;

  std::replace(method.begin(), method.end(), ' ', '-');

  if (args["--output-format"].asString() == "gnuplot")
    file_extension = "dat";

  if (args["--output-format"].asString() == "png")
    file_extension = "png";

  return base + "-" + method + "-" + nodes + "-" + reflex + "-" + convex + "." + file_extension;
}

void collectSimpleStats(std::function<void(docopt::Arguments &)> func, docopt::Arguments &args) {
  TIME_START
  func(args);
  TIME_END
  TIME_PARSE
}
