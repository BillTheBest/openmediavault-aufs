<?php
/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 * @copyright Copyright (c) 2013-2014 OpenMediaVault Plugin Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
require_once("openmediavault/object.inc");
require_once("openmediavault/config.inc");
require_once("openmediavault/error.inc");
require_once("openmediavault/util.inc");
require_once("openmediavault/rpcservice.inc");
require_once("openmediavault/notify.inc");

class OMVRpcServiceAufs extends OMVRpcServiceAbstract {
    /**
     * Get the RPC service name.
     */
    public function getName() {
        return "aufs";
    }

    /**
     * Initialize the RPC service.
     */
    public function initialize() {
        $this->registerMethod("getShareList");
        $this->registerMethod("getShare");
        $this->registerMethod("setShare");
        $this->registerMethod("deleteShare");
    }

    public function getShareList($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "start"     : { "type" : "integer" },
                "limit"     : { '.$GLOBALS['OMV_JSONSCHEMA_COUNTFIELD'].' },
                "sortfield" : { '.$GLOBALS['OMV_JSONSCHEMA_SORTFIELD'].' },
                "sortdir"   : { '.$GLOBALS['OMV_JSONSCHEMA_SORTDIR'].' }
            }
        }');

        // Get the configuration object.
        $xpath = "//services/aufs/share";
        $objects = $xmlConfig->getList($xpath);
        if(is_null($objects))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $xpath);

        // Add additional share informations.
        foreach($objects as $objectk => &$objectv) {
            $branches = "";

            for($i=0; $i<10; $i++) {
                $sfRef = "sharedfolderref".$i;
                $sfUuid = $objectv[$sfRef];

                if(empty($sfUuid))
                    continue;

                // Get the shared folder configuration object.
                $sfObject = $xmlConfig->get("//system/shares/sharedfolder[uuid='{$objectv[$sfRef]}']");
                if(!is_null($sfObject) && !empty($sfObject))
                    $branches .= $sfObject['name']."<br/>";
            }

            $objectv['branches'] = $branches;
        }

        // Filter the result.
        return $this->applyFilter($objects, $params['start'], $params['limit'],
            $params['sortfield'], $params['sortdir']);
    }

    /**
     * Get a share config object.
     * @param params An array containing the following fields:
     *   \em uuid The UUID of the configuration object.
     * @param context The context of the caller.
     * @return The requested configuration object.
     * @throw E_CONFIG_GET_OBJECT_FAILED
     */
    public function getShare($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "uuid" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID'].' }
            }
        }');

        // Get the configuration object.
        $xpath = sprintf("//services/aufs/share[uuid='%s']", $params['uuid']);
        $object = $xmlConfig->get($xpath);
        if(is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $xpath);

        // Modify the result object.
        $object['guestok']    = boolval($object['guestok']);
        $object['readonly']   = boolval($object['readonly']);
        $object['browseable'] = boolval($object['browseable']);
        $object['mfs']        = boolval($object['mfs']);
        $object['udba']       = boolval($object['udba']);

        return $object;
    }

    /**
     * Set a share config object.
     * @param params The method parameters.
     * @param context The context of the caller.
     * @return The stored configuration object.
     * @throw E_CONFIG_OBJECT_UNIQUENESS
     * @throw E_CONFIG_GET_OBJECT_FAILED
     * @throw E_CONFIG_SET_OBJECT_FAILED
     */
    public function setShare($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "uuid"             : { '.$GLOBALS['OMV_JSONSCHEMA_UUID_UNDEFINED'].' },
                "poolname"         : { "type" : "string" },
                "sharedfolderbind" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID_NONE'].', "optional" : true },
                "smbsharename"     : { "type" : "string", "optional" : true },
                "guestok"          : { "type" : "boolean" },
                "readonly"         : { "type" : "boolean" },
                "browseable"       : { "type" : "boolean" },
                "sharedfolderref0" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID'].' },
                "sharedfolderref1" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID'].' },
                "sharedfolderref2" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID_NONE'].', "optional" : true },
                "sharedfolderref3" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID_NONE'].', "optional" : true },
                "sharedfolderref4" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID_NONE'].', "optional" : true },
                "sharedfolderref5" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID_NONE'].', "optional" : true },
                "sharedfolderref6" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID_NONE'].', "optional" : true },
                "sharedfolderref7" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID_NONE'].', "optional" : true },
                "sharedfolderref8" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID_NONE'].', "optional" : true },
                "sharedfolderref9" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID_NONE'].', "optional" : true },
                "mfs"              : { "type" : "boolean" },
                "udba"             : { "type" : "boolean" }
            }
        }');

        touch("/tmp/aufs_log.txt");

        $bindUuid = $params["sharedfolderbind"];

        // Look for duplicates
        if(!empty($bindUuid)) {
            if(TRUE === $xmlConfig->exists("//services/aufs/share[" . "sharedfolderbind='{$bindUuid}']")) {
                throw new OMVException(OMVErrorMsg::E_CONFIG_OBJECT_UNIQUENESS,
                    gettext("This bind share is already in use by branch in another pool."));
            }
        }

        for($i=0; $i<10; $i++) {
            $sfRef = "sharedfolderref".$i;
            $sfUuid = $params[$sfRef];

            if(empty($sfUuid))
                continue;

            if(TRUE === $xmlConfig->exists("//services/aufs/share[" . "{$sfRef}='{$bindUuid}']")) {
                throw new OMVException(OMVErrorMsg::E_CONFIG_OBJECT_UNIQUENESS,
                    gettext("This bind share is already in use by branch in another pool."));
            }

            if($sfUuid === $bindUuid) {
                throw new OMVException(OMVErrorMsg::E_CONFIG_OBJECT_UNIQUENESS,
                    sprintf(gettext("The same shared folder is used by bind share and #%s."), ($i+1) ));
            }

            for($j=9; $j>$i; $j--) {
                $sfRef2 = "sharedfolderref".$j;
                $sfUuid2 = $params[$sfRef2];

                if($sfUuid === $sfUuid2) {
                    throw new OMVException(OMVErrorMsg::E_CONFIG_OBJECT_UNIQUENESS,
                        sprintf(gettext("The same shared folder is used by branch #%s and #%s."),
                        ($i+1), ($j+1) ));
                }
            }
        }

        // Prepare the configuration object.
        $object = array(
            "uuid"             => ($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) ? OMVUtil::uuid() : $params['uuid'],
            "poolname"         => $params['poolname'],
            "sharedfolderbind" => $params['sharedfolderbind'],
            "smbsharename"     => $params['smbsharename'],
            "guestok"          => array_boolval($params, 'guestok'),
            "readonly"         => array_boolval($params, 'readonly'),
            "browseable"       => array_boolval($params, 'browseable'),
            "sharedfolderref0" => $params['sharedfolderref0'],
            "sharedfolderref1" => $params['sharedfolderref1'],
            "sharedfolderref2" => $params['sharedfolderref2'],
            "sharedfolderref3" => $params['sharedfolderref3'],
            "sharedfolderref4" => $params['sharedfolderref4'],
            "sharedfolderref5" => $params['sharedfolderref5'],
            "sharedfolderref6" => $params['sharedfolderref6'],
            "sharedfolderref7" => $params['sharedfolderref7'],
            "sharedfolderref8" => $params['sharedfolderref8'],
            "sharedfolderref9" => $params['sharedfolderref9'],
            "mfs"              => array_boolval($params, 'mfs'),
            "udba"             => array_boolval($params, 'udba')
        );

        // Create fstab line
        // none /storage aufs br:/media/disk1=rw:/media/disk2=rw,sum,create=mfs,udba=notify 0 0
        $sf = "";

        if($object['mfs'] === true)
            $mfs = "mfs";
        else
            $mfs = "pmfs";

        if($object['udba'] === true)
            $udba = "reval";
        else
            $udba = "notify";

        for($i=0; $i<10; $i++) {
            $sfRef = "sharedfolderref".$i;
            $sfUuid = $object[$sfRef];

            if(empty($sfUuid))
                continue;

            // Get the absolute shared folder path.
            $sfPath = OMVRpc::exec("ShareMgmt", "getPath", array("uuid" => $params[$sfRef]), $context);
            $sf .= ":".$sfPath."=rw";
        }
        $object['fstab_line'] = "none /media/".$object['poolname']." aufs br".$sf.",sum,create=".$mfs.",udba=".$udba." 0 0";

        // Make pooling directory
        if( ! file_exists("/media/".$object['poolname']) )
            mkdir("/media/".$object['poolname']);

        // Create bind fstab line
        // /media/poolname  /media/uuid/sharedfoldername  none  bind  0 0
        if(!empty($bindUuid)) {
            $sfPath = OMVRpc::exec("ShareMgmt", "getPath", array("uuid" => $bindUuid), $context);
            $object['fstab_bind'] = "/media/".$object['poolname']."  ".$sfPath."  none  bind  0 0";
        }

        // Set the configuration object.
        $success = FALSE;
        if($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) {
            // Check pool name uniqueness.
            if(TRUE === $xmlConfig->exists("//services/aufs/share[poolname='{$params['poolname']}']")) {
                throw new OMVException(OMVErrorMsg::E_CONFIG_OBJECT_UNIQUENESS,
                    sprintf(gettext("A pool with the name '%s' already exists."),
                    $params['poolname']));
            }

            // Check samba share name uniqueness.
            if(TRUE === $xmlConfig->exists("//services/aufs/share[smbsharename='{$params['smbsharename']}']")) {
                throw new OMVException(OMVErrorMsg::E_CONFIG_OBJECT_UNIQUENESS,
                    sprintf(gettext("A aufs sambashare with the name '%s' already exists."),
                    $params['smbsharename']));

            }

            if(TRUE === $xmlConfig->exists("//services/smb/shares/share[name='{$params['smbsharename']}']")) {
                throw new OMVException(OMVErrorMsg::E_CONFIG_OBJECT_UNIQUENESS,
                    sprintf(gettext("A Samba share with the name '%s' already exists."),
                    $params['smbsharename']));
            }

            // Append the configuration object.
            $success = $xmlConfig->set("//services/aufs", array("share" => $object));
        } else {
            // Update the existing configuration object.
            $success = $xmlConfig->replace("//services/aufs/share[uuid='{$params['uuid']}']", $object);
        }
        if(FALSE === $success)
            throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED);

        // Notify configuration changes.
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) ?
            OMV_NOTIFY_CREATE : OMV_NOTIFY_MODIFY,
            "org.openmediavault.services.aufs.share", $object);

        // Return the configuration object.
        return $object;
    }

    /**
     * Delete a share configuration object.
     * @param params An array containing the following fields:
     *   \em uuid The UUID of the configuration object.
     * @param context The context of the caller.
     * @return The deleted configuration object.
     */
    public function deleteShare($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "uuid" : { '.$GLOBALS['OMV_JSONSCHEMA_UUID'].' }
            }
        }');

        // Get the configuration object.
        $xpath = sprintf("//services/aufs/share[uuid='%s']", $params['uuid']);
        $object = $xmlConfig->get($xpath);
        if(is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $xpath);

        $cmd = sprintf("export LANG=C; umount /media/%s", $object['poolname']);
        $this->exec($cmd);

        $cmd = sprintf("export LANG=C; rmdir /media/%s", $object['poolname']);
        $this->exec($cmd);

        // Delete the configuration object.
        $object = $this->deleteConfigObjectByPath($xpath,
            "org.openmediavault.services.aufs.share");

        // Apply the changes immediately (no user interaction is wanted).
        OMVRpc::exec("Config", "applyChanges", array(
            "modules" => array("aufs"),
            "force" => TRUE
        ), $context);

        // Return the deleted configuration object.
        return $object;
    }
}

// Register the RPC service.
$rpcServiceMgr = &OMVRpcServiceMgr::getInstance();
$rpcServiceMgr->registerService(new OMVRpcServiceAufs());
?>
