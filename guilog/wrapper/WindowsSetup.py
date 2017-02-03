# To create, python WindowsSetup.py py2exe

from distutils.core import setup
import py2exe
import Config

setup(
    version = Config.version,
    description = "ingimp Windows launcher",
    name = "ingimp",

    # targets to build
    windows = [{ "script":"ingimp",
                 "icon_resources":[(1,"../../app/wilber.ico")]
              }],
    console = ["ingimp-helper.py"],
#    options = { 'py2exe': {'excludes' : ['Config'] } },
    )
