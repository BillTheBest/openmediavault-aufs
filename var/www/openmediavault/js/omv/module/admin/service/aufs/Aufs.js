/**
 *
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
// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/grid/Panel.js")
// require("js/omv/workspace/window/Form.js")
// require("js/omv/workspace/window/plugin/ConfigObject.js")
// require("js/omv/Rpc.js")
// require("js/omv/data/Store.js")
// require("js/omv/data/Model.js")
// require("js/omv/data/proxy/Rpc.js")
// require("js/omv/form/field/SharedFolderComboBox.js")

Ext.define("OMV.module.admin.service.aufs.Share", {
    extend : "OMV.workspace.window.Form",
    uses   : [
        "OMV.form.field.SharedFolderComboBox",
        "OMV.workspace.window.plugin.ConfigObject"
    ],

    rpcService   : "aufs",
    rpcGetMethod : "getShare",
    rpcSetMethod : "setShare",
    plugins      : [{
        ptype : "configobject"
    }],

    width        : 500,

    getFormItems: function() {
        var me = this;
        return [{
            xtype    : "fieldset",
            title    : _("Pool Settings"),
            defaults : {
                labelSeparator : ""
            },
            items : [{
                xtype      : "textfield",
                name       : "poolname",
                fieldLabel : _("Pool Name"),
                allowBlank : false,
                plugins    : [{
                    ptype : "fieldinfo",
                    text  : _("A directory will be created and mounted in /media/ using Pool Name.")
                }]
            }]
        },{
            xtype    : "fieldset",
            title    : _("Samba Settings"),
            defaults : {
                labelSeparator : ""
            },
            items : [{
                xtype      : "textfield",
                name       : "smbsharename",
                fieldLabel : _("Samba Share"),
                allowBlank : true,
                plugins    : [{
                    ptype : "fieldinfo",
                    text  : _("Samba share name for pool. Leave blank to disable.")
                }]
            },{
                xtype      : "checkbox",
                name       : "guestok",
                fieldLabel : _("Public"),
                checked    : false,
                boxLabel   : _("If enabled then no password is required to connect to the share")
            },{
                xtype      : "checkbox",
                name       : "readonly",
                fieldLabel : _("Read only"),
                checked    : false,
                boxLabel   : _("Set read only"),
                plugins    : [{
                    ptype : "fieldinfo",
                    text  : _("If this parameter is set, then users may not create or modify files in the share.")
                }]
            },{
                xtype      : "checkbox",
                name       : "browseable",
                fieldLabel : _("Browseable"),
                checked    : true,
                boxLabel   : _("Set browseable"),
                plugins    : [{
                    ptype : "fieldinfo",
                    text  : _("This controls whether this share is seen in the list of available shares in a net view and in the browse list.")
                }]
            }]
        },{
            xtype    : "fieldset",
            title    : _("Branches"),
            defaults : {
                labelSeparator : ""
            },
            items : [{
                border : false,
                html   : "<p>" + _("Pool must contain at least two branches.") + "</p>"
            },{
                xtype      : "sharedfoldercombo",
                name       : "sharedfolderref0",
                fieldLabel : _("Branch #1")
            },{
                xtype      : "sharedfoldercombo",
                name       : "sharedfolderref1",
                fieldLabel : _("Branch #2")
            },{
                xtype      : "sharedfoldercombo",
                name       : "sharedfolderref2",
                fieldLabel : _("Branch #3"),
                allowBlank : true
            },{
                xtype      : "sharedfoldercombo",
                name       : "sharedfolderref3",
                fieldLabel : _("Branch #4"),
                allowBlank : true
            },{
                xtype      : "sharedfoldercombo",
                name       : "sharedfolderref4",
                fieldLabel : _("Branch #5"),
                allowBlank : true
            },{
                xtype      : "sharedfoldercombo",
                name       : "sharedfolderref5",
                fieldLabel : _("Branch #6"),
                allowBlank : true
            },{
                xtype      : "sharedfoldercombo",
                name       : "sharedfolderref6",
                fieldLabel : _("Branch #7"),
                allowBlank : true
            },{
                xtype      : "sharedfoldercombo",
                name       : "sharedfolderref7",
                fieldLabel : _("Branch #8"),
                allowBlank : true
            },{
                xtype      : "sharedfoldercombo",
                name       : "sharedfolderref8",
                fieldLabel : _("Branch #9"),
                allowBlank : true
            },{
                xtype      : "sharedfoldercombo",
                name       : "sharedfolderref9",
                fieldLabel : _("Branch #10"),
                allowBlank : true
            }]
        },{
            xtype    : "fieldset",
            title    : _("Advanced Settings"),
            defaults : {
                labelSeparator : ""
            },
            items : [{
                xtype      : "checkbox",
                name       : "mfs",
                fieldLabel : _("MFS"),
                checked    : false,
                plugins    : [{
                    ptype : "fieldinfo",
                    text  : _("If unchecked, pmfs will be used. If checked, mfs will be used. See Info tab for explanation.")
                }]
            },{
                xtype      : "checkbox",
                name       : "udba",
                fieldLabel : _("UDBA"),
                checked    : true,
                plugins    : [{
                    ptype : "fieldinfo",
                    text  : _("If unchecked, notify will be used.  If checked, reval will be used. See Info tab for explanation.")
                }]
            }]
        }];
    }
});

Ext.define("OMV.module.admin.service.aufs.Shares", {
    extend   : "OMV.workspace.grid.Panel",
    requires : [
        "OMV.Rpc",
        "OMV.data.Store",
        "OMV.data.Model",
        "OMV.data.proxy.Rpc"
    ],
    uses     : [
        "OMV.module.admin.service.aufs.Share"
    ],

    hidePagingToolbar : false,
    stateful          : true,
    stateId           : "9889057b-b1c0-4c48-a4c1-8c8b4fb54d7b",
    columns           : [{
        text      : _("Pool Name"),
        sortable  : true,
        dataIndex : "poolname",
        stateId   : "poolname"
    },{
        text      : _("Samba Share"),
        sortable  : true,
        dataIndex : "smbsharename",
        stateId   : "smbsharename"
    },{
        text      : _("Branches"),
        sortable  : true,
        dataIndex : "branches",
        stateId   : "branches"
    }],

    initComponent : function() {
        var me = this;
        Ext.apply(me, {
            store : Ext.create("OMV.data.Store", {
                autoLoad : true,
                model    : OMV.data.Model.createImplicit({
                    idProperty  : "uuid",
                    fields      : [
                        { name : "uuid", type: "string" },
                        { name : "poolname", type: "string" },
                        { name : "smbsharename", type: "string" },
                        { name : "branches", type: "string" }
                    ]
                }),
                proxy : {
                    type    : "rpc",
                    rpcData : {
                        service : "aufs",
                        method  : "getShareList"
                    }
                }
            })
        });
        me.callParent(arguments);
    },

    onAddButton : function() {
        var me = this;
        Ext.create("OMV.module.admin.service.aufs.Share", {
            title     : _("Add share"),
            uuid      : OMV.UUID_UNDEFINED,
            listeners : {
                scope  : me,
                submit : function() {
                    this.doReload();
                }
            }
        }).show();
    },

    onEditButton : function() {
        var me = this;
        var record = me.getSelected();
        Ext.create("OMV.module.admin.service.aufs.Share", {
            title     : _("Edit share"),
            uuid      : record.get("uuid"),
            listeners : {
                scope  : me,
                submit : function() {
                    this.doReload();
                }
            }
        }).show();
    },

    doDeletion : function(record) {
        var me = this;
        OMV.Rpc.request({
            scope    : me,
            callback : me.onDeletion,
            rpcData : {
                service : "aufs",
                method  : "deleteShare",
                params  : {
                    uuid : record.get("uuid")
                }
            }
        });
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "shares",
    path      : "/service/aufs",
    text      : _("Pools"),
    position  : 10,
    className : "OMV.module.admin.service.aufs.Shares"
});

OMV.WorkspaceManager.registerNode({
    id      : "aufs",
    path    : "/service",
    text    : _("Pooling"),
    icon16  : "images/aufs.png",
    iconSvg : "images/aufs.svg"
});
