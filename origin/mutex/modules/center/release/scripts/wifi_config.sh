#!/bin/sh

start_wpa() {
    if ! pgrep wpa_supplicant >/dev/null; then
        wpa_supplicant -Dnl80211 -iwlan0 -c /data/misc/wifi/wpa_supplicant.conf &
    else
        echo "wpa_supplicant is running"
        exit 0
    fi
    exit 0
}

check_wpa() {
    for i in $(seq 1 10); do
        if pgrep wpa_supplicant >/dev/null; then
            # echo "wpa_supplicant is running"
            break
        fi
        sleep 1
    done

    if [ $i -eq 10 ]; then
        echo "wpa start failed"
        exit -10
    fi
}

enable_wifi() {
    check_wpa
    output=$(wpa_cli -iwlan0 list_networks)

    awkRes=$(echo "$output" | awk 'NR>1 && $1=="0" {print $1}')
    if ! echo "$awkRes" | grep -q "0"; then
        output=$(wpa_cli -iwlan0 add_network)
        if ! echo "$output" | grep -q "0"; then
            echo "Failed to add Network ID 0."
            exit -1
        fi
    fi

    ssid=\"$1\"
    psk=\"$2\"

    output=$(wpa_cli -iwlan0 status)
    if echo "$output" | grep -q "wpa_state=INACTIVE"; then
        echo "network don't need to disable"
    else
        wpa_cli -iwlan0 disable_network 0
    fi

    sleep 1

    output=$(wpa_cli -iwlan0 set_network 0 ssid ${ssid})
    if echo "$output" | grep -q "OK"; then
        output=$(wpa_cli -iwlan0 set_network 0 psk $psk)
        if ! echo "$output" | grep -q "OK"; then
            echo "set psk false"
            exit -2
        fi
    else
        echo "set ssid false"
        exit -3
    fi

    output=$(wpa_cli -iwlan0 enable_network 0)
    if ! echo "$output" | grep -q "OK"; then
        echo "enable wifi failed"
        exit -4
    fi
    echo "enable wifi success"
    exit 0
}

check_status() {
    check_wpa
    for i in $(seq 1 30); do
        output=$(wpa_cli -iwlan0 status)
        if echo "$output" | grep -q "wpa_state=COMPLETED"; then
            break
        fi
        sleep 1
    done

    if [ $i -eq 30 ]; then
        echo "wifi connection timeout"
        exit -5
    fi

    output=$(wpa_cli -iwlan0 save_config)
    if ! echo "$output" | grep -q "OK"; then
        echo "save wifi config failed"
        exit -6
    fi
    echo "wifi connected and save config success"
    exit 0
}

restart_wpa() {
    killall wpa_supplicant >/dev/null
    sleep 1
    wpa_supplicant -Dnl80211 -iwlan0 -c /data/misc/wifi/wpa_supplicant.conf &
    exit 0
}

### --------------------------------------------------------------------
### Scirpt params
### --------------------------------------------------------------------
case "$1" in
start_wpa)
    start_wpa
    ;;
enable_wifi)
    enable_wifi $2 $3
    ;;
check_status)
    check_status
    ;;
restart_wpa)
    restart_wpa
    ;;
*)
    exit 1
    ;;
esac
