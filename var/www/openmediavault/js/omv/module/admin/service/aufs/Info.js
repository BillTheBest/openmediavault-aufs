/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    OpenMediaVault Plugin Developers
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
// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/panel/Panel.js")

Ext.define("OMV.module.admin.service.aufs.Info", {
    extend : "Ext.panel.Panel",

    initComponent: function() {
        var me = this;

        me.html = "<b>" + _("User's Direct Branch Access (UDBA)") + "</b>" +
                  "<p>" +
                    _("UDBA means a modification to a branch filesystem manually or directly, e.g. bypassing aufs. While aufs is designed and implemented to be safe after UDBA, it can make yourself and your aufs confused. And some information like aufs inode will be incorrect. For example, if you rename a file on a branch directly, the file on aufs may or may not be accessible through both of old and new name. Because aufs caches various information about the files on branches. And the cache still remains after UDBA. Aufs has a mount option named 'udba' which specifies the test level at access time whether UDBA was happened or not.") +
                  "</p>" +
                  "<ul>" +
                    "<li>" +
                      _("udba=reval") +
                      "<br/><br/>" +
                      _("Aufs tests only the existence of the file which existed. If the existed file was removed on the branch directly, aufs discard the cache about the file and re-lookup it. So the data will be updated. This test is at minimum level to keep the performance and ensure the existence of a file. This is default and aufs runs still fast.") +
                    "</li>" +
                    "<li>" +
                      _("udba=notify") +
                      "<br/><br/>" +
                      _("Aufs sets either 'fsnotify' or 'inotify' to all the accessed directories on its branches and receives the event about the dir and its children. It consumes resources, cpu and memory. Performance will be hurt, but it is most strict test level. When a user accesses the file which was notified UDBA before, the cached data about the file will be discarded and aufs will re-lookup it.") +
                    "</li>" +
                  "</ul>" +
                  "<b>" + _("Most-Free-Space (MFS)") + "</b>" +
                  "<ul>" +
                    "<li>" +
                      _("create=mfs") +
                      "<br/><br/>" +
                      _("Selects a writable branch which has most free space.") +
                    "</li>" +
                    "<li>" +
                      _("create=pmfs") +
                      "<br/><br/>" +
                      _("Selects a writable branch where the parent dir exists. When the parent dir exists on multiple writable branches, aufs selects the one which has more free space.") +
                    "</li>" +
                  "</ul>";
        me.callParent(arguments);
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "info",
    path      : "/service/aufs",
    text      : _("Information"),
    position  : 20,
    className : "OMV.module.admin.service.aufs.Info"
});
