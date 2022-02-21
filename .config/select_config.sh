source .config/setup.sh

val=$(dialog --keep-tite --menu "Select build configuration" 0 $DIALOGWIDTH 2 d Debug r Release c Custom 2>&1 1>/dev/tty)
case $? in
	0)
		case $val in
			d)
				cp .config/debug.mk $BUILDCONFIG
			;;
			r)
				cp .config/release.mk $BUILDCONFIG
			;;
			c)
				.config/custom_config.sh
			;;
		esac
	;;

	*)
		echo "Cancelled - No changes where made!"
	;;
esac
