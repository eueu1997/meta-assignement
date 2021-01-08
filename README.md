cd poky
source oe-init-build-env build_qemuarm

download in poky folder the meta-assignement folder

bitbake-layers add-layer ../meta-assignement
echo "IMAGE_INSTALL_append = " hrm"" >> local.conf
echo "IMAGE_INSTALL_append = " hrm-app"" >> local.conf
echo "KERNEL_MODULE_AUTOLOAD  = " hrm"" >> local.conf
bitbake core-image-minimal

runqemu qemuarm

launch the app with the command : hrm-app
